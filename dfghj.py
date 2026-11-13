import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path


BASE = Path(__file__).resolve().parent


def find_exe() -> str:
    """Cherche l'exécutable dans le projet."""
    for folder in (BASE, BASE / "build", BASE / "src", BASE.parent / "build", BASE.parent / "src"):
        for name in ("main.exe", "main"):
            if (folder / name).is_file():
                return str(folder / name)
    return ""


EXE = ""
TIMEOUT = 5

# Active les couleurs ANSI dans le terminal Windows et force l'UTF-8 en sortie
os.system("")
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")

GREEN = "\033[92m"
RED = "\033[91m"
YELLOW = "\033[93m"
RESET = "\033[0m"


def ensure_exe() -> str:
    """Compile main.c avant les tests ou utilise un binaire à jour si aucun compilateur n'existe."""
    source = BASE / "main.c"
    if not source.is_file():
        print(f"{RED}Fichier source introuvable : {source}{RESET}")
        sys.exit(1)

    existing_exe = find_exe()
    compiler = shutil.which("gcc") or shutil.which("clang")
    executable = BASE / ("main.exe" if os.name == "nt" else "main")
    if compiler is not None:
        command = [compiler, str(source), "-o", str(executable)]
        use_shell = False
    else:
        vs_roots = (
            Path(os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)"))
            / "Microsoft Visual Studio",
            Path(os.environ.get("ProgramFiles", r"C:\Program Files"))
            / "Microsoft Visual Studio",
        )
        vcvars = next(
            (
                path
                for root in vs_roots
                for path in root.glob("*/*/VC/Auxiliary/Build/vcvarsall.bat")
            ),
            None,
        )
        if vcvars is None:
            if existing_exe and Path(existing_exe).stat().st_mtime_ns >= source.stat().st_mtime_ns:
                return existing_exe
            print(f"{RED}Exécutable introuvable et aucun compilateur C disponible.{RESET}")
            print(f"Compile {source} avec un compilateur C, puis relance ce script.")
            sys.exit(1)

        command = f'call "{vcvars}" x64 >nul && cl /nologo /Fe:"{executable}" "{source}"'
        use_shell = True

    try:
        result = subprocess.run(
            command,
            shell=use_shell,
            cwd=BASE,
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
        )
    except OSError as error:
        print(f"{RED}Impossible de lancer le compilateur : {error}{RESET}")
        sys.exit(1)

    if result.returncode != 0:
        print(f"{RED}Échec de la compilation de main.c :{RESET}")
        print(result.stdout, end="")
        print(result.stderr, end="")
        sys.exit(result.returncode)

    print(f"{GREEN}Compilation réussie : {executable}{RESET}")
    return str(executable)


def test_file_pair(
    in_file: Path,
    out_file: Path,
    error_limit: int | None,
    errors: int,
) -> tuple[bool, int, bool]:
    print(f"\n--- Test : {in_file.parent.name}/{in_file.name} ---")
    inputs = in_file.read_text(encoding="utf-8")
    expected = out_file.read_text(encoding="utf-8").splitlines()

    try:
        result = subprocess.run(
            [EXE],
            input=inputs,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            cwd=BASE,
            text=True,
            encoding="utf-8",
            errors="replace",
            timeout=TIMEOUT,
        )
    except subprocess.TimeoutExpired:
        print(f"    TIMEOUT : le programme ne s'est pas terminé en {TIMEOUT}s")
        errors += 1
        stopped = error_limit is not None and errors >= error_limit
        return False, errors, stopped
    except OSError as error:
        print(f"{RED}Impossible de lancer l'exécutable {EXE} : {error}{RESET}")
        if getattr(error, "winerror", None) == 4551:
            print("Windows bloque le binaire avec une stratégie de contrôle d'application.")
        errors += 1
        stopped = error_limit is not None and errors >= error_limit
        return False, errors, stopped

    received = result.stdout.splitlines()
    test_errors = 0
    line_errors = 0
    max_lines = max(len(received), len(expected))
    for i in range(max_lines):
        got = received[i] if i < len(received) else None
        want = expected[i] if i < len(expected) else None

        if got == want:
            print(f"Réponse : {got!r} -> {GREEN}CORRECT{RESET}")
            continue

        errors += 1
        test_errors += 1
        line_errors += 1
        print(f"Réponse : {got!r} -> {RED}ERREUR{RESET}")
        print(f"  🎯 Attendu : {want!r}")
        if error_limit is not None and errors >= error_limit:
            print(f"{YELLOW}Limite de {error_limit} erreur(s) atteinte.{RESET}")
            return False, errors, True

    if result.returncode != 0:
        errors += 1
        test_errors += 1
        print(f"{RED}Le programme s'est terminé avec le code {result.returncode}.{RESET}")
        if error_limit is not None and errors >= error_limit:
            print(f"{YELLOW}Limite de {error_limit} erreur(s) atteinte.{RESET}")
            return False, errors, True

    ok = max_lines - line_errors
    print(f"\n{ok}/{max_lines} lignes correctes pour ce fichier")

    stopped = error_limit is not None and errors >= error_limit
    return test_errors == 0, errors, stopped


def sprint_tests(sprint: int) -> list[tuple[Path, Path]]:
    # Adaptation à votre structure d'arborescence (dossier inout/ ou racine)
    sprint_directory = BASE / "inout"
    if not sprint_directory.is_dir():
        sprint_directory = BASE

    tests = []
    for in_file in sorted(sprint_directory.glob(f"in-sp{sprint}-*.txt")):
        out_file = in_file.with_name(in_file.name.replace("in-", "out-", 1))
        if not out_file.is_file():
            print(f"{RED}Fichier attendu introuvable : {out_file}{RESET}")
            continue
        tests.append((in_file, out_file))
    return tests


def choose_tests() -> list[tuple[Path, Path]]:
    print("Que voulez-vous tester ?")
    print("1. Tous les sprints (1 à 4)")
    print("2. Un sprint spécifique")
    print("3. Reprendre à partir d'un sprint")
    print("5. Le sujet de base (in.txt / out.txt)")

    choice = input("Votre choix [1] : ").strip() or "1"

    if choice == "1":
        return [
            test
            for sprint in range(1, 5)
            for test in sprint_tests(sprint)
        ]

    if choice == "2":
        sprint = input("Numéro du sprint (1 à 4) : ").strip()
        if sprint not in {"1", "2", "3", "4"}:
            raise ValueError("Le numéro du sprint doit être compris entre 1 et 4.")
        return sprint_tests(int(sprint))

    if choice == "3":
        sprint = input("Reprendre à partir du sprint (1 à 4) : ").strip()
        if sprint not in {"1", "2", "3", "4"}:
            raise ValueError("Le numéro du sprint doit être compris entre 1 et 4.")
        return [
            test
            for sprint_number in range(int(sprint), 5)
            for test in sprint_tests(sprint_number)
        ]

    if choice == "5":
        return [(BASE / "in.txt", BASE / "out.txt")]

    raise ValueError("Choix invalide.")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Teste main.c avec les fichiers in/out.")
    parser.add_argument(
        "--sprint",
        type=int,
        choices=range(1, 5),
        help="lance directement les tests d'un sprint (1 à 4)",
    )
    return parser.parse_args()


def choose_error_limit() -> int | None:
    value = input(
        "Nombre maximal d'erreurs avant arrêt (0 = illimité) [0] : "
    ).strip() or "0"
    try:
        limit = int(value)
    except ValueError as error:
        raise ValueError("Le nombre d'erreurs doit être un entier positif ou nul.") from error
    if limit < 0:
        raise ValueError("Le nombre d'erreurs doit être positif ou nul.")
    return limit or None


def main():
    global EXE
    args = parse_args()
    try:
        tests = sprint_tests(args.sprint) if args.sprint is not None else choose_tests()
        error_limit = choose_error_limit()
    except ValueError as error:
        print(f"{RED}{error}{RESET}")
        sys.exit(1)

    if not tests:
        print(f"{RED}Aucun test trouvé. Vérifiez l'emplacement de vos fichiers in/out.{RESET}")
        sys.exit(1)

    EXE = ensure_exe()

    success = True
    errors = 0
    total_global_tests = 0
    total_global_errors = 0

    for in_file, out_file in tests:
        # Extrait le sprint si possible pour structurer l'affichage
        test_success, errors, stopped = test_file_pair(
            in_file, out_file, error_limit, errors
        )
        total_global_tests += 1
        if not test_success:
            success = False
            total_global_errors += 1
        if stopped:
            print(f"{YELLOW}Arrêt des tests après {errors} erreur(s).{RESET}")
            break

    print(f"\n==============================")
    correct_tests = total_global_tests - total_global_errors
    print(f"{correct_tests}/{total_global_tests} fichiers de tests corrects")
    
    if success:
        print(GREEN + "Tous les tests sélectionnés sont réussis." + RESET)
        sys.exit(0)
    else:
        print(RED + "Certains tests ont échoué." + RESET)
        sys.exit(1)


if __name__ == "__main__":
    main()
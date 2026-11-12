import os
import queue
import subprocess
import sys
import threading
from pathlib import Path


BASE = Path(__file__).resolve().parent


def find_exe() -> str:
    """Cherche l'exécutable dans build/ puis src/ (Windows : main.exe, sinon main)."""
    for folder in (BASE.parent / "build", BASE.parent / "src", BASE):
        for name in ("main.exe", "main"):
            if (folder / name).is_file():
                return str(folder / name)
    return str(BASE / "main.exe")


EXE = find_exe()
TIMEOUT = 5
RESPONSE_TIMEOUT = 0.1

# Active les couleurs ANSI dans le terminal Windows et force l'UTF-8 en sortie
os.system("")
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")

GREEN = "\033[92m"
RED = "\033[91m"
YELLOW = "\033[93m"
RESET = "\033[0m"


class Program:
    """Lance le programme C et lit sa sortie ligne par ligne (compatible Windows)."""

    def __init__(self, exe: str, cwd: str):
        self.proc = subprocess.Popen(
            [exe],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            cwd=cwd,
            text=True,
            encoding="utf-8",
            errors="replace",
            bufsize=1,
        )
        self.lines: "queue.Queue[str | None]" = queue.Queue()
        self.eof = False
        threading.Thread(target=self._reader, daemon=True).start()

    def _reader(self):
        try:
            for line in self.proc.stdout:
                self.lines.put(line.rstrip("\r\n"))
        finally:
            self.lines.put(None)  # None = fin du flux

    def send(self, line: str) -> bool:
        try:
            self.proc.stdin.write(line + "\n")
            self.proc.stdin.flush()
            return True
        except (BrokenPipeError, OSError, ValueError):
            return False

    def read_available(self) -> list[str]:
        response = []
        while not self.eof:
            try:
                item = self.lines.get(timeout=RESPONSE_TIMEOUT)
            except queue.Empty:
                break
            if item is None:
                self.eof = True
                break
            response.append(item)
        return response

    def read_until_exit(self) -> list[str] | None:
        response = []
        while not self.eof:
            try:
                item = self.lines.get(timeout=TIMEOUT)
            except queue.Empty:
                return None
            if item is None:
                self.eof = True
                break
            response.append(item)
        return response

    def close(self):
        try:
            self.proc.stdin.close()
        except Exception:
            pass
        try:
            self.proc.wait(timeout=1)
        except subprocess.TimeoutExpired:
            self.proc.kill()


def test_file_pair(
    in_file: Path,
    out_file: Path,
    error_limit: int | None,
    errors: int,
) -> tuple[bool, int, bool]:
    print(f"\n--- Test : {in_file.parent.name}/{in_file.name} ---")
    inputs = in_file.read_text(encoding="utf-8").splitlines()
    expected = out_file.read_text(encoding="utf-8").splitlines()

    if not Path(EXE).is_file():
        print(f"{RED}Exécutable introuvable : {EXE}{RESET}")
        print("Compile d'abord ton programme (ex. : gcc main.c -o main.exe).")
        sys.exit(1)

    child = Program(EXE, str(BASE))

    received = []
    test_errors = 0
    for i, line in enumerate(inputs):
        child.send(line)

        if line.strip().upper() == "EXIT":
            response = child.read_until_exit()
            if response is None:
                print(f"    TIMEOUT : le programme ne s'est pas terminé en {TIMEOUT}s")
                child.close()
                errors += 1
                return False, errors, error_limit is not None and errors >= error_limit
        else:
            response = child.read_available()

        received.extend(response)
        first_result_index = len(received) - len(response)
        for response_index, result in enumerate(response):
            result_index = first_result_index + response_index
            want = expected[result_index] if result_index < len(expected) else None
            
            if result == want:
                status = GREEN + "CORRECT" + RESET
                print(f"Réponse : {result!r} -> {status}")
            else:
                errors += 1
                test_errors += 1
                status = RED + "ERREUR" + RESET
                print(f"Réponse : {result!r} -> {status}")
                print(f"  🎯 Attendu : {want!r}")
                if error_limit is not None and errors >= error_limit:
                    print(f"{YELLOW}Limite de {error_limit} erreur(s) atteinte.{RESET}")
                    child.close()
                    return False, errors, True

    child.close()
    ok = sum(got == want for got, want in zip(received, expected))
    max_lines = max(len(received), len(expected))
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
    try:
        tests = choose_tests()
        error_limit = choose_error_limit()
    except ValueError as error:
        print(f"{RED}{error}{RESET}")
        sys.exit(1)

    if not tests:
        print(f"{RED}Aucun test trouvé. Vérifiez l'emplacement de vos fichiers in/out.{RESET}")
        sys.exit(1)

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
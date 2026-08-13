# Compiler Project

## Description
In this project, we are going to be building an AOT (Ahead-Of-Time) compiler for the programming language C.

---

## Rules

### Commits
* **Atomic Commits:** Enforce the use of atomic commits. Make small, incremental commits that are easy to follow.
* **Commit Message Format:** To keep the git log history clean, use the following format for all commit messages:
  `type: short atomic description`

#### Available Types:
* `feat`: updating or adding implementation
* `fix`: fixing an error/bug
* `docs`: updating documentation
* `test`: writing/adding unit/integration tests
* `eliminate`: eliminating obselete file from the code base
> **Note:** You can combine types if necessary (e.g., `fix(docs): correct typo in build instructions`).

---

### Branches

#### Naming Conventions
* Keep names descriptive. 
* If a branch name contains more than one word, use underscores to separate them (e.g., `feature_lexic_parser`).

#### Merging & Pull Requests
* **Code Review:** Always use pull requests so that more than one person reviews the code before it merges into `main`.
* **Stability:** Under no circumstances can we merge into `main` if the code does not compile and pass tests. The `main` branch is for production-ready code only.
* **Cleanup:** After merging (and making sure everything runs well), delete the branch.




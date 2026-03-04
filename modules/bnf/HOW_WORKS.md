# BNF Engine Workflow

This document describes the global workflow of the system based on the custom BNF formalism, from parsing the language definition file to lexing a target file (e.g., a configuration file).

---

# 1. Step 1: Parsing the Language Definition File

The **Semantic Descriptor** is responsible for parsing the language definition file written in BNF.

## 1.1 Concept Construction

During this stage:

- The BNF file is parsed.
- A **concept map** is built.
- Each concept contains:
  - One or more **definitions**
  - Each definition is an **ordered sequence of rules**

The possible rule types are:

- `Symbol`
- `Sequence`
- `Option`
- `Group`

The elements inside these rules can be:

- A **Terminal Char** (e.g., `'a'`, `'hello'`)
- A **Concept** (a reference to another concept)

---

## 1.2 Structural Validation

Once the grammar is fully parsed, the Semantic Descriptor performs several critical validations:

### ✔ Presence of `<head>`

- `<head>` must exist.
- It is the unique entry point of the language.

### ✔ No Recursion

- No concept may be recursive.
- This includes indirect recursion.
- This constraint guarantees deterministic and finite traversal.

---

## 1.3 Link Resolution

If the grammar is valid:

- All concept references are resolved.
- Rules are connected together using **internal pointers**.
- An optimized tree structure is built.

This allows the second stage (lexing) to navigate efficiently through the rule tree without expensive dynamic lookups.

---

# 2. Step 2: Lexing a Target File

Once the grammar is compiled and validated, it is used to analyze a target file (e.g., a configuration file).

## 2.1 File Reading

The lexer:

1. Reads the file character by character.
2. At each character, evaluates the possible validation paths.

---

## 2.2 Path Exploration

The system operates using a set of **active paths**:

- Initially, a single path is created starting from the `<head>` concept.
- For each character read:
  - All currently valid paths are evaluated.
  - **New paths** are generated from rules compatible with the current character.
  - Incompatible paths are discarded.
  - Valid paths continue to the next character.

This mechanism enables:

- Parallel exploration of possibilities
- Progressive validation of the file
- Automatic elimination of invalid branches

---

## 2.3 Final Validation Condition

The file is considered **valid** if, at the end of the input stream:

- The `<head>` concept is fully validated  
  **or**
- All remaining rules are optional

If no valid path remains before reaching the end of the file, the file is considered invalid.
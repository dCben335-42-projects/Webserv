# Custom BNF Formalism Documentation

This document describes the structure and rules of the custom BNF grammar used for parsing files.

---

# 1. Fundamental Principles

## 1.1 Root Concept (`<head>`)

- `<head>` is the **single and mandatory entry point** of the grammar.

- No other concept can reference `<head>`.

- All parsing begins exclusively with this concept.


## 1.2 Recursion

- Recursive concepts are **strictly forbidden**.

- This includes any form of recursion, even if a theoretical stopping condition exists.

- This restriction ensures deterministic parsing and avoids potential infinite loops.

## 1.3 Assigning Concepts

A concept is defined in two steps:

1. Creating its first rule with `::=`
2. Adding possible alternatives with `|=` (Optional)

### Example

```bnf
ABSOLUTE_PATH       ::= '/' { SEGMENT }
                    |= '/'
                    |= DQUOTE '/' { SEGMENT_QUOTED } DQUOTE
```

## 2. Rules

The grammar is based on four fundamental structures for validating tokens.

Let A and B be two concepts.

| Syntax | Type | Functioning |
| :--- | :--- | :--- |
| `A` | **Symbol** | Appel simple d'une règle définie ailleurs. |
| `[ A \| B ]` | **Option** | 0 ou 1 élément parmi la liste fournie. |
| `{ A \| B }` | **Sequence** | Répétition de 1 à N fois (itération linéaire). |
| `( A \| B )` | **Group** | Sélectionne exactement 1 élément parmi les choix. |

*The symbol | represents a logical OR. (It is not mandatory but recommended)*

## 3. Terminal Character

A terminal represents a printable ASCII string directly present in the input.

- ```'h'```
- ```'hello'```

### Rules

- Terminals are always enclosed in single quotes (').
- They can contain:
    - A single character
    - Multiple characters
- They represent an exact literal match in the parsed stream.
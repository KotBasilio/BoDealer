# AGENTS.md — BoDealer collaboration policy

This file defines default instructions for AI agents working in this repository.

## Scope
- Applies to the entire repository rooted at this directory.

## Collaboration mode
- Do not create pull requests unless explicitly requested by the user.
- Commit directly to the current working branch.
- Prefer small, focused commits with clear commit messages.

## Safety and change scope
- Touch only files required for the requested task.
- Avoid broad refactors unless explicitly requested.
- Preserve existing behavior unless the user asks for functional changes.

## C++ editing conventions
- Keep changes minimal and style-consistent with surrounding code.
- Do not introduce new dependencies without explicit approval.
- Do not add ornamental comments or banners.

## Validation
- Run the smallest relevant checks available for the changed area.
- If a check cannot run due to environment limitations, report that clearly.

## Communication
- Summarize what changed, why, and how it was validated.
- Flag assumptions and uncertainties explicitly.

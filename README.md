# MiniGit

MiniGit is a minimal version control system written in **C++**.

It implements core version-control concepts such as:
- Repository initialization
- File staging
- Commit snapshots
- Commit history (`log`)
- State restoration (`checkout`)

## Build

g++ main.cpp -o mygit

## Usage

mygit init
mygit add <file>
mygit commit "message"
mygit log
mygit checkout <id>

## About

This project was built to understand how version control systems work internally,
including persistent state management and file-system–based snapshots.



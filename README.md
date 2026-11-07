<div align="center">
🔍 DupFinder
Find duplicate files and free up your disk space

A fast command-line tool written in C that finds identical files by their content, not by their names, and safely removes the extra copies.

Show Image Show Image Show Image

✨ Features • 📥 Download • 🚀 Usage • ⚙️ How it works • ⚠️ Warnings

</div>
🤔 What problem does it solve?

Over time, folders like Downloads fill up with the same file saved again and again: the same PDF, the same photo, the same installer.

The names are often different (report.pdf, report (1).pdf, report_final.pdf), so you can't see them. DupFinder ignores the names and compares the real content, so it finds every exact copy.

✨ Features
	Feature
📂	Scans a folder and all its subfolders
🧬	Finds identical files even with different names
📊	Shows duplicates in groups
💾	Tells you how much space is wasted
🗑️	Deletes the extra copies and keeps one of each
✅	Asks for confirmation before deleting anything
⚡	Fast: only reads the full content of files with the same size
💻	Works on Windows, Linux and macOS
🎯 Where can you use it?
📥 Cleaning your Downloads folder
🖼️ Cleaning folders full of photos and screenshots
🎓 Organizing university or work files you copied many times
🧹 Freeing space on your laptop
📥 Download

Option 1: Download the ZIP (easiest)

Click the green Code button at the top of this page
Click Download ZIP
Unzip it

Option 2: Use git

git clone https://github.com/ouelaamohamed1-lab/DupFinder.git
🔧 Compile

You need a C compiler (gcc). Open a terminal inside the project folder (the one containing dupfinder.c), then follow your system:

🐧 Linux (Ubuntu / Debian)
sudo apt install build-essential
gcc -Wall -o dupfinder dupfinder.c
🍎 macOS
xcode-select --install
gcc -Wall -o dupfinder dupfinder.c
🪟 Windows
Install a compiler: MSYS2 (then run pacman -S mingw-w64-ucrt-x86_64-gcc in its terminal) or w64devkit
Open a terminal in the project folder and run:
gcc -Wall -o dupfinder.exe dupfinder.c

✅ Done! The program is ready.

🚀 Usage
👀 See the duplicates (nothing is deleted)
System	Command
🐧 Linux / 🍎 macOS	./dupfinder ~/Downloads
🪟 Windows	dupfinder.exe C:\Users\YourName\Downloads
🗑️ See the duplicates, then delete the extra copies
System	Command
🐧 Linux / 🍎 macOS	./dupfinder ~/Downloads --delete
🪟 Windows	dupfinder.exe C:\Users\YourName\Downloads --delete

The program shows the list and asks:

Delete the copies and keep one file per group? (y/n)

Type y to delete or n to cancel.

💡 Tip: To scan another folder, just replace the path. If the path has spaces, put it in quotes: "C:\Users\Your Name\Pictures"

📸 Example output
Scanning test ...
Found 7 files.

Group 1  (12.00 B each)
  keep:   test/sub/a_copy.txt
  copy:   test/sub/deep/a_copy2.txt
  copy:   test/a.txt

Group 2  (97.66 KB each)
  keep:   test/sub/big_copy.bin
  copy:   test/big.bin

3 duplicate files, wasting 97.68 KB.
Run again with --delete to remove the copies.
Word	Meaning
keep	✅ The file that stays
copy	🗑️ An extra copy that --delete will remove
⚙️ How it works

DupFinder checks files in 3 steps, from the fastest to the slowest:

Step	What it does	Why
1️⃣ Size	Compares file sizes	Files with different sizes can't be identical, so they are skipped instantly
2️⃣ Hash	Calculates a number from the file content	Identical files always give the same number
3️⃣ Byte by byte	Compares the real content	Makes sure the files are 100% identical

This way the program never wastes time reading every file completely. ⚡

⚠️ Warnings

[!WARNING] Deleted files do not go to the trash. They are gone for good.

💾 Make a backup first if the files are important (just copy the folder)
👀 Run the program without --delete first and read the list
📌 The program keeps the first file it finds in each group, which may not be the one you prefer
🧠 What I learned building it
📁 Working with folders and files in C
🧮 Dynamic memory (realloc, free)
🔁 Recursion (scanning folders inside folders)
🔢 Hashing and sorting (qsort)
⌨️ Reading command-line arguments
🌍 Writing code that works on different operating systems
🛣️ Future ideas
 Choose which copy to keep (the oldest or the shortest path)
 Move copies to a folder instead of deleting them
 Option to ignore small files (--min-size)
 A stronger hash such as SHA-256
 Support for folder names with special characters on Windows

Current limits: ignores shortcuts (symbolic links) and empty files.

👤 Author

Ouelaa Mohamed Said: Electronics student

💬 Found a bug or have an idea? Open an Issue or send a suggestion. ⭐ If you like the project, give it a star!

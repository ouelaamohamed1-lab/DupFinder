DupFinder

A small tool written in C that finds duplicate files on your computer and helps you delete the extra copies to free up space.

What does it do?

Over time, folders like Downloads fill up with the same file saved many times: the same PDF, the same photo, the same installer. The names are often different (report.pdf, report (1).pdf, report_final.pdf), so it's hard to notice.

DupFinder ignores the names and looks at the content of the files. If two files are exactly the same, it tells you.

Features
Scans a folder and all the folders inside it
Finds files with exactly the same content, even if the names are different
Shows the duplicates in groups
Tells you how much space is wasted
Can delete the extra copies and keep one file from each group
Asks you for confirmation before deleting anything
Fast: it only reads the full content of files that have the same size
Where can you use it?
Cleaning your Downloads folder
Cleaning folders full of photos or screenshots
Organizing university or work files you copied many times
Freeing space on your laptop

Works on Windows, Linux and macOS.

How to download it

First download the project: on this page click the green Code button, then Download ZIP, and unzip it. (If you use git, you can run git clone https://github.com/ouelaamohamed1-lab/DupFinder.git instead.)

Then open a terminal inside the project folder (the one that contains dupfinder.c).

Then compile it. You need a C compiler (gcc).

Linux (Ubuntu / Debian)
sudo apt install build-essential
gcc -Wall -o dupfinder dupfinder.c
macOS
xcode-select --install
gcc -Wall -o dupfinder dupfinder.c
Windows
Install a compiler, for example MSYS2 (then run pacman -S mingw-w64-ucrt-x86_64-gcc in its terminal) or w64devkit.
Open a terminal in the project folder and run:
gcc -Wall -o dupfinder.exe dupfinder.c

That's it. The program is ready.

How to use it

To see the duplicates in a folder (nothing is deleted):

Linux / macOS:

./dupfinder ~/Downloads

Windows:

dupfinder.exe C:\Users\YourName\Downloads

To see the duplicates and then delete the extra copies:

Linux / macOS:

./dupfinder ~/Downloads --delete

Windows:

dupfinder.exe C:\Users\YourName\Downloads --delete

The program shows the list and asks:

Delete the copies and keep one file per group? (y/n)

Type y and press Enter to delete, or n to cancel.

To scan any other folder, replace the folder path with the one you want. If the path has spaces, put it in quotes: "C:\Users\Your Name\Pictures".

Example output
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
keep is the file that stays.
copy is an extra copy that --delete will remove.
Important warnings
Deleted files do not go to the trash. They are gone for good.
Make a backup first if the files are important (just copy the folder).
Run the program without --delete first and read the list.
The program keeps the first file it finds in each group. This might not be the one you prefer.
How does it work?

It uses three simple steps, from fastest to slowest:

Compare sizes. Two files with different sizes can't be the same, so they are skipped right away.
Compare hashes. A hash is a number calculated from the content of a file. Identical files always give the same number.
Compare byte by byte. To be 100% sure, the program compares the real content of the files that passed steps 1 and 2.

This way it doesn't waste time reading every file completely.

What I learned building it
Working with folders and files in C
Dynamic memory (realloc, free)
Recursion (scanning folders inside folders)
Hashing and sorting (qsort)
Reading command-line arguments
Limitations and ideas for the future
Doesn't follow shortcuts (symbolic links) and ignores empty files
On Windows, folder names with special characters (like Arabic letters) may not be read
Choose which copy to keep (oldest or shortest path)
Move copies to a folder instead of deleting them
Option to ignore small files (--min-size)
A stronger hash such as SHA-256
Author

Made by Ouelaa Mohamed Said, electronics student. Feel free to open an issue or suggest an improvement.

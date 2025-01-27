#Linux-Timemachine
requirements:
linux c++17 compiler
linux operating sytem with diff command installed 

TODO:
add logic for handling running out of space
Add functionality for config file to allow scheduling repeat backups
with every x backups(with changes)  being a full backup 
Add functionality to allow being run on startup automatically 
Add file tree to be stored with each backup so that deleted files can be seen (or diffs of file tree however that might not be worth the effort since file tree so be small) 
(and if no file tree then assume previous as a fallback)
(could do +path or -path to show changes to tree from last backup)
store file trees in a folder called filetree/ with name of tree just name of backup as a text file 
Add compression for backed up data


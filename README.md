# Introduction
Our goal is to create and maintain "Very Vanilla Live", a ban-safe and reliable version of MacroQuest2 made with Redguides' community plugins, modifications, and macros.

We're currently supporting the Test client in this gitlab, but all plugins and macros will be merged with our Live and RoF (emu) versions.

# Getting Started
Information about installing Very Vanilla can be found on RedGuides.com:

https://www.redguides.com/forums/threads/26389-MacroQuest2-Beginner-s-Guide-(start-here)

# Build and Test Very Vanilla
1) Install Visual Studio 2017 (free edition)

2) Team -> Manage Connections

3) Under "Local Git Repositories" Click "Clone". 

4) Enter this url: https://gitlab.com/redguides/VeryVanilla.git
Specify a local folder to place the files, and click "Clone". If it's your first time cloning, you'll be asked for your GitLab login & password.

5) Go to the cloned folder. Run MQ2auth.exe

6) Open RG_Vanilla.sln

7) Retarget projects? Click OK

8) Build -> Configuration Manager -> switch from debug to release

9) Build -> Clean Solution

10) Build -> Build Solution

11) You did it! You'll find the goodies in the release directory. If you want MQ2Nav it needs to be compiled on its own, see brainiac's instructions on github.

If you're new to compiling, you can find more detailed instructions here:

http://www.macroquest2.com/wiki/index.php/MacroQuest2:Compiling


# Contribute
We love contributors! If you're testing changes that might break things, create or request a new branch for your project. Otherwise feel free to commit to the master branch.

Make sure you add your plugins to the Plugin List, and your macro to the Macro List on RedGuides.com to attract more users.

Not everything has to be MQ2 related, we've hosted many other programs for EverQuest in the past and we want to see them here.

## Git Guide
If you're working on a single macro by yourself, you don't have to learn anything. You can edit the macro directly on gitlab.com - no programs required.


To add or update a macro, it's as simple as this:

1) Go to https://gitlab.com/redguides/VeryVanilla/tree/master/Release/Macros

2) click "+" then "upload file"


If you're working with others, or with several files, it's best to use software to avoid conflicting edits.

We have a guide on using Git with Visual Studio here:
https://www.redguides.com/forums/threads/55499-Using-git-with-Visual-Studio-2017-absolute-beginners

If you want more detail, GitHub has a nice little tutorial:
https://help.github.com/articles/set-up-git/

You can even use their "GitHub Desktop" client with our Gitlab repository, as Redguides member erfg12 shows here:
https://newagesoldier.com/how-to-use-github-windows-client-with-gitlab-reposaccounts/

Most developers in the MQ2 scene use Visual Studio Community Edition to connect to git, which is also free.

# Access

If you'd like to keep your work private, a special plugin for example, or if you need access to code not shown here, send a PM to Redbot. 

If there are plugins you don't see here that you'd like to work on, please PM Redbot. test




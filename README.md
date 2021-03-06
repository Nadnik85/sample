# Introduction
Our goal is to maintain "Very Vanilla", a safe version of the open source project MacroQuest2, along with Redguides community add-ons. We do not allow active hacks, cheats, or any mechanic that's otherwise impossible in-game.

We're currently supporting the Test client in this gitlab, but changes are regularly merged with our Live and Emu versions.

# Not a developer?
If you're not a developer, you'll find our pre-compiled versions on RedGuides.com much easier to use:

https://www.redguides.com/community/resources/very-vanilla-mq2-live-servers.61/

# Reporting Issues
Please submit issues to the "discussion" tab on the resource's page here:
https://www.redguides.com/community/resources/

If the issue is a security concern, please report here:
https://www.redguides.com/community/misc/contact/

# How to Build and Test Very Vanilla
1) Install Visual Studio (community edition is free!)

2) Team -> Manage Connections

3) Under "Local Git Repositories" Click "Clone". 

4) Enter this url: https://gitlab.com/redguides/VeryVanilla.git
Specify a local folder to place the files, and click "Clone". If it's your first time cloning, you'll be asked for your GitLab login & password.

5) Go to the cloned folder. Run MQ2auth.exe and save.

6) Open VanillaDIY.sln

7) Retarget projects? Click OK

8) Go to MQ2Main -> Header Files -> MQ2Main.h and if there's a line that says "#define LIVE" change it to "#define TEST" 

9) Build -> Configuration Manager -> switch from debug to release

10) Build -> Clean Solution

11) Build -> Build Solution

12) You did it! You'll find the goodies in the release directory. If you want MQ2Nav it needs to be compiled on its own, see brainiac's instructions on github.

## Contributing
We love contributions! If you have something to add to Very Vanilla, please use the "Forking X" workflow as described here,
https://www.redguides.com/community/threads/git-very-vanilla-forking-x-workflow.70582/

If you'd like to contribute privately or work on something that isn't included in this repo, please contact Redbot. We're always happy to have help!






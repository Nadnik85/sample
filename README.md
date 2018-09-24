# Introduction
Our goal is to maintain "Very Vanilla", a safe version of the open source project MacroQuest2 with Redguides' community plugins and modifications. We do not allow active hacks, cheats, or any mechanic that's otherwise impossible in-game.

We're currently supporting the Test client in this gitlab, but changes are regularly merged with our Live and Emu versions.

# Not a developer?
If you're not a developer, you'll find our pre-compiled versions on RedGuides.com much easier to use:

https://www.redguides.com/community/resources/very-vanilla-mq2-live-servers.61/

# How to Build and Test Very Vanilla
1) Install Visual Studio 2017 (community edition is free!)

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

# Contribute
We love contributors. 

Not everything has to be MQ2 related, we've hosted many other programs for EverQuest. Feel free to add them here in their own folder, see "EQBackupRestore" for an example.

If you want to make a private/paid contribution to the project, please contact Redbot. Otherwise you agree to the DCO:

Developer Certificate of Origin
Version 1.1

Copyright (C) 2004, 2006 The Linux Foundation and its contributors.
1 Letterman Drive
Suite D4700
San Francisco, CA, 94129

Everyone is permitted to copy and distribute verbatim copies of this
license document, but changing it is not allowed.


Developer's Certificate of Origin 1.1

By making a contribution to this project, I certify that:

(a) The contribution was created in whole or in part by me and I
    have the right to submit it under the open source license
    indicated in the file; or

(b) The contribution is based upon previous work that, to the best
    of my knowledge, is covered under an appropriate open source
    license and I have the right under that license to submit that
    work with modifications, whether created in whole or in part
    by me, under the same open source license (unless I am
    permitted to submit under a different license), as indicated
    in the file; or

(c) The contribution was provided directly to me by some other
    person who certified (a), (b) or (c) and I have not modified
    it.

(d) I understand and agree that this project and the contribution
    are public and that a record of the contribution (including all
    personal information I submit with it, including my sign-off) is
    maintained indefinitely and may be redistributed consistent with
    this project or the open source license(s) involved.

Full license details can be found in LICENSE.md

## Git Guide

We have a guide on using Git with Visual Studio here:
https://www.redguides.com/community/threads/using-git-with-visual-studio-2017-absolute-beginners.55499/

If you want more detail, GitHub has a nice little tutorial:
https://help.github.com/articles/set-up-git/

# Access

If you'd like to keep your work private or if you need access to code not shown here, send a PM to Redbot. 

Plugin you need to work on that we don't include here? PM Redbot.




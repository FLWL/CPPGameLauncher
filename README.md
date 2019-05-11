# CPPGameLauncher

A game launcher/updater/patcher written in C++ for the Windows platform.

For the accompanying patch creator, see: https://github.com/FLWL/CPPGameLauncher_PatchCreator

# Example screenshot

![Example screenshot](https://raw.githubusercontent.com/FLWL/CPPGameLauncher/master/misc/example_screenshot.png)

# Features

* Download application files over HTTP connection
* Reduce download size by using compressed .gz files
* Verify the origin of the remote files using RSA public key
* Verify the integrity of downloaded files using SHA-256 hashes
* Determine which files need to be updated by comparing CRC hashes and file sizes
* Support multiple remote servers
* Launcher can update itself
* Repair installation function
* Show news in launcher
* About page
* Keep track of installed files in an ini file for uninstallers

# Dependencies

* CRC++ (included)
* ini-parser (included)
* OpenSSL
* zlib
* curl

# Installation
1. Install Visual Studio 2017 with "Desktop development with C++" option and Windows 10 SDK
2. Clone this repository, open the .sln file in Visual Studio
3. Compile or acquire static libraries for zlib, OpenSSL and curl.
4. From project properties in VS, make sure that a valid Windows SDK is selected, and correct the library and include search paths for aforementioned libraries.
5. Configure the launcher by editing the "constants.h" file. Change the domain, public keys, etc. Also take look at the patch creator's repository and its readme: https://github.com/FLWL/CPPGameLauncher_PatchCreator

While the patch creator needs the private key in .der format, the launcher has hardcoded .pem public keys in its "constants.h". To generate a readable .pem format from the keypair.pem file generated during the patch creator setup, run the following command:

```openssl rsa -in keypair.pem -outform PEM -pubout -out public.pem```

Open the resulting public.pem file with a text editor, and copy the contents to constants.h into the variable "MAIN_PUBLIC_KEY", adding newlines and quotation marks as needed. It is also possible to use another backup key pair by repeating the whole generation process and putting the resulting public key file contents to the AUXILIARY_PUBLIC_KEY variable. The launcher will accept files signed with either one of the keys.

6. Compile the application, set up the patch creator, create a patch, upload it to the web server, test the launcher.

# The remote update server layout
By default the CPPGameLauncher is configured to look for 1 update server, with the address of update1.example.com. It tries to find the patch files from http://update1.example.com/patch/. The files created by the patch creator must be uploaded to that folder, so that the launcher could access http://update1.example.com/patch/latest.txt and other files. The domain can be changed from "constants.h".

If you change the "NUMBER_OF_UPDATE_SERVERS" variable in constants.h to 3 for example, then the launcher will try to use http://update2.example.com/patch/ and so on in case the first update server is not responding.

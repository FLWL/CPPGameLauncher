# CPPGameLauncher

A game launcher/updater/patcher written in C++ for the Windows platform.

# Example screenshot

![Example screenshot](https://raw.githubusercontent.com/huvolk/CPPGameLauncher/master/misc/example_screenshot.png)

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

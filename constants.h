#pragma once

#include <string>

namespace Constants
{
	static const int NUMBER_OF_UPDATE_SERVERS = 1;
	static const int LAUNCHER_VERSION = 102;
	static const std::wstring LAUNCHER_VERSION_STRING = L"1.02";

	static const std::wstring GAME_EXECUTABLE_NAME = L"Game.exe";
	static const std::wstring GAME_LAUNCHER_FILE_NAME = L"CPPGameLauncher.exe";

	static const int FILE_BUFFER_SIZE = 64 * 1024;
	static const std::string UPDATE_DOMAIN = ".example.com";
	static const std::string UPDATE_DIRECTORY = "/patch";

	static const std::wstring VERSION_FILE = L"\\CPPGameLauncher.ini";
	static const std::wstring UNINSTALL_FILE = L"\\unins.ini"; // a file to write list of installed game files in, for the uninstaller

	static const std::string REMOTE_NEWS_FILE = "/news.txt";
	static const std::string REMOTE_NEWS_SIGNATURE_FILE = "/news.sig";

	static const std::string REMOTE_VERSION_FILE = "/latest.txt";
	static const std::string REMOTE_VERSION_SIGNATURE_FILE = "/latest.sig";
	static const std::string REMOTE_CHECKSUMS_FILE = "/checksums.txt";
	static const std::string REMOTE_CHECKSUMS_SIGNATURE_FILE = "/checksums.sig";
	static const std::string REMOTE_LAUNCHER_EXECUTABLE_FILE = "/latest_launcher.exe";

	static const std::string MAIN_PUBLIC_KEY =
		"-----BEGIN PUBLIC KEY-----\n"\
		"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAz2BVH/W95NW1FzxY3jD0\n"\
		"8oe4WnLJyJbOUyCfEuF808tGXpbrZJa/WXWDBJbeaf1XQhyLBP8cizo5AmEATRQz\n"\
		"/eNCec6ecadDNNMdDBQqDZzqx4ZgO6nYfqx8z0MSwFDuMgXp9L09kXh9Uu520td/\n"\
		"X19WWTY1IIvEWgux9pOLkb4VxmUM4g65LACNGhdSB5YB+Sk6McJHofye2+Q6Msh8\n"\
		"XYVpK0THBLNyop5QDoto5IUkgI+aWPyITOPM06z9XM7gwShxIYTn+a7rN0zZDn+O\n"\
		"Z15VwrU6l1Ou0w63w4KtRfblP2aXc29sm3hjOLn+4P6gjcGpqeH99nQVNvEs5TQh\n"\
		"cwIDAQAB\n"\
		"-----END PUBLIC KEY-----\n";
	static const std::string AUXILIARY_PUBLIC_KEY =
		"-----BEGIN PUBLIC KEY-----\n"\
		"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAz2BVH/W95NW1FzxY3jD0\n"\
		"8oe4WnLJyJbOUyCfEuF808tGXpbrZJa/WXWDBJbeaf1XQhyLBP8cizo5AmEATRQz\n"\
		"/eNCec6ecadDNNMdDBQqDZzqx4ZgO6nYfqx8z0MSwFDuMgXp9L09kXh9Uu520td/\n"\
		"X19WWTY1IIvEWgux9pOLkb4VxmUM4g65LACNGhdSB5YB+Sk6McJHofye2+Q6Msh8\n"\
		"XYVpK0THBLNyop5QDoto5IUkgI+aWPyITOPM06z9XM7gwShxIYTn+a7rN0zZDn+O\n"\
		"Z15VwrU6l1Ou0w63w4KtRfblP2aXc29sm3hjOLn+4P6gjcGpqeH99nQVNvEs5TQh\n"\
		"cwIDAQAB\n"\
		"-----END PUBLIC KEY-----\n";

	static const std::wstring GUID = L"Launcher{910A5365-6AA9-4332-AC4B-65F133BD73A3}";

	static const int LINES_ON_ABOUT_SCREEN = 17;
	static const std::wstring ABOUT_TEXT =
		L"CPPGameLauncher v" + LAUNCHER_VERSION_STRING + L"\n"\
		"Copyright (c) 2017-2018\n"\
		"All rights reserved.\n"\
		"\n"\
		"--- Terms of Use ---\n"\
		"\n"\
		"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"\
		"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"\
		"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL\n"\
		"THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"\
		"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"\
		"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN\n"\
		"THE SOFTWARE.\n"\
		"\n"\
		"--- Third-Party Licenses ---\n"\
		"\n"\
		"--- CRC++ ---\n"\
		"\n"\
		"Copyright (c) 2016, Daniel Bahr\n"\
		"All rights reserved.\n"\
		"\n"\
		"Redistribution and use in source and binary forms, with or without\n"\
		"modification, are permitted provided that the following conditions are met:\n"\
		"\n"\
		"* Redistributions of source code must retain the above copyright notice, this\n"\
		"  list of conditions and the following disclaimer.\n"\
		"\n"\
		"* Redistributions in binary form must reproduce the above copyright notice,\n"\
		"  this list of conditions and the following disclaimer in the documentation\n"\
		"  and/or other materials provided with the distribution.\n"\
		"\n"\
		"* Neither the name of CRC++ nor the names of its\n"\
		"  contributors may be used to endorse or promote products derived from\n"\
		"  this software without specific prior written permission.\n"\
		"\n"\
		"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"\n"\
		"AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n"\
		"IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n"\
		"DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE\n"\
		"FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL\n"\
		"DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR\n"\
		"SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\n"\
		"CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,\n"\
		"OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE\n"\
		"OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n"\
		"\n"\
		"--- curl ---\n"\
		"\n"\
		"COPYRIGHT AND PERMISSION NOTICE\n"\
		"\n"\
		"Copyright (c) 1996 - 2017, Daniel Stenberg, <daniel@haxx.se>, and many\n"\
		"contributors, see the THANKS file.\n"\
		"\n"\
		"All rights reserved.\n"\
		"\n"\
		"Permission to use, copy, modify, and distribute this software for any purpose\n"\
		"with or without fee is hereby granted, provided that the above copyright\n"\
		"notice and this permission notice appear in all copies.\n"\
		"\n"\
		"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"\
		"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"\
		"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN\n"\
		"NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,\n"\
		"DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR\n"\
		"OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE\n"\
		"OR OTHER DEALINGS IN THE SOFTWARE.\n"\
		"\n"\
		"Except as contained in this notice, the name of a copyright holder shall not\n"\
		"be used in advertising or otherwise to promote the sale, use or other dealings\n"\
		"in this Software without prior written authorization of the copyright holder.\n"\
		"\n"\
		"--- iniparser ---\n"\
		"\n"\
		"The MIT License (MIT)\n"\
		"Copyright (c) <2015> <carriez.md@gmail.com>\n"\
		"\n"\
		"Permission is hereby granted, free of charge, to any person obtaining a copy\n"\
		"of this software and associated documentation files (the \"Software\"), to\n"\
		"deal in the Software without restriction, including without limitation the\n"\
		"rights to use, copy, modify, merge, publish, distribute, sublicense, and/or\n"\
		"sell copies of the Software, and to permit persons to whom the Software is\n"\
		"furnished to do so, subject to the following conditions:\n"\
		"\n"\
		"The above copyright notice and this permission notice shall be included in\n"\
		"all copies or substantial portions of the Software.\n"\
		"\n"\
		"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"\
		"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"\
		"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"\
		"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"\
		"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"\
		"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN\n"\
		"THE SOFTWARE.\n"\
		"\n"\
		"--- OpenSSL ---\n"\
		"\n"\
		"LICENSE ISSUES\n"\
		"==============\n"\
		"\n"\
		"The OpenSSL toolkit stays under a double license, i.e. both the conditions of\n"\
		"the OpenSSL License and the original SSLeay license apply to the toolkit.\n"\
		"See below for the actual license texts.\n"\
		"\n"\
		"OpenSSL License\n"\
		"---------------\n"\
		"\n"\
		"/* ====================================================================\n"\
		"* Copyright (c) 1998-2017 The OpenSSL Project.  All rights reserved.\n"\
		"*\n"\
		"* Redistribution and use in source and binary forms, with or without\n"\
		"* modification, are permitted provided that the following conditions\n"\
		"* are met:\n"\
		"*\n"\
		"* 1. Redistributions of source code must retain the above copyright\n"\
		"*    notice, this list of conditions and the following disclaimer. \n"\
		"*\n"\
		"* 2. Redistributions in binary form must reproduce the above copyright\n"\
		"*    notice, this list of conditions and the following disclaimer in\n"\
		"*    the documentation and/or other materials provided with the\n"\
		"*    distribution.\n"\
		"*\n"\
		"* 3. All advertising materials mentioning features or use of this\n"\
		"*    software must display the following acknowledgment:\n"\
		"*    \"This product includes software developed by the OpenSSL Project\n"\
		"*    for use in the OpenSSL Toolkit. (http://www.openssl.org/)\"\n"\
		"*\n"\
		"* 4. The names \"OpenSSL Toolkit\" and \"OpenSSL Project\" must not be used to\n"\
		"*    endorse or promote products derived from this software without\n"\
		"*    prior written permission. For written permission, please contact\n"\
		"*    openssl-core@openssl.org.\n"\
		"*\n"\
		"* 5. Products derived from this software may not be called \"OpenSSL\"\n"\
		"*    nor may \"OpenSSL\" appear in their names without prior written\n"\
		"*    permission of the OpenSSL Project.\n"\
		"*\n"\
		"* 6. Redistributions of any form whatsoever must retain the following\n"\
		"*    acknowledgment:\n"\
		"*    \"This product includes software developed by the OpenSSL Project\n"\
		"*    for use in the OpenSSL Toolkit (http://www.openssl.org/)\"\n"\
		"*\n"\
		"* THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY\n"\
		"* EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n"\
		"* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR\n"\
		"* PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR\n"\
		"* ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,\n"\
		"* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT\n"\
		"* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n"\
		"* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)\n"\
		"* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,\n"\
		"* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)\n"\
		"* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED\n"\
		"* OF THE POSSIBILITY OF SUCH DAMAGE.\n"\
		"* ====================================================================\n"\
		"*\n"\
		"* This product includes cryptographic software written by Eric Young\n"\
		"* (eay@cryptsoft.com).  This product includes software written by Tim\n"\
		"* Hudson (tjh@cryptsoft.com).\n"\
		"*\n"\
		"*/\n"\
		"\n"\
		"Original SSLeay License\n"\
		"-----------------------\n"\
		"\n"\
		"/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)\n"\
		"* All rights reserved.\n"\
		"*\n"\
		"* This package is an SSL implementation written\n"\
		"* by Eric Young (eay@cryptsoft.com).\n"\
		"* The implementation was written so as to conform with Netscapes SSL.\n"\
		"* \n"\
		"* This library is free for commercial and non-commercial use as long as\n"\
		"* the following conditions are aheared to.  The following conditions\n"\
		"* apply to all code found in this distribution, be it the RC4, RSA,\n"\
		"* lhash, DES, etc., code; not just the SSL code.  The SSL documentation\n"\
		"* included with this distribution is covered by the same copyright terms\n"\
		"* except that the holder is Tim Hudson (tjh@cryptsoft.com).\n"\
		"* \n"\
		"* Copyright remains Eric Young's, and as such any Copyright notices in\n"\
		"* the code are not to be removed.\n"\
		"* If this package is used in a product, Eric Young should be given attribution\n"\
		"* as the author of the parts of the library used.\n"\
		"* This can be in the form of a textual message at program startup or\n"\
		"* in documentation (online or textual) provided with the package.\n"\
		"* \n"\
		"* Redistribution and use in source and binary forms, with or without\n"\
		"* modification, are permitted provided that the following conditions\n"\
		"* are met:\n"\
		"* 1. Redistributions of source code must retain the copyright\n"\
		"*    notice, this list of conditions and the following disclaimer.\n"\
		"* 2. Redistributions in binary form must reproduce the above copyright\n"\
		"*    notice, this list of conditions and the following disclaimer in the\n"\
		"*    documentation and/or other materials provided with the distribution.\n"\
		"* 3. All advertising materials mentioning features or use of this software\n"\
		"*    must display the following acknowledgement:\n"\
		"*    \"This product includes cryptographic software written by\n"\
		"*     Eric Young (eay@cryptsoft.com)\"\n"\
		"*    The word 'cryptographic' can be left out if the rouines from the library\n"\
		"*    being used are not cryptographic related :-).\n"\
		"* 4. If you include any Windows specific code (or a derivative thereof) from \n"\
		"*    the apps directory (application code) you must include an acknowledgement:\n"\
		"*    \"This product includes software written by Tim Hudson (tjh@cryptsoft.com)\"\n"\
		"* \n"\
		"* THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND\n"\
		"* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n"\
		"* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE\n"\
		"* ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE\n"\
		"* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL\n"\
		"* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS\n"\
		"* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)\n"\
		"* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT\n"\
		"* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY\n"\
		"* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF\n"\
		"* SUCH DAMAGE.\n"\
		"* \n"\
		"* The licence and distribution terms for any publically available version or\n"\
		"* derivative of this code cannot be changed.  i.e. this code cannot simply be\n"\
		"* copied and put under another distribution licence\n"\
		"* [including the GNU Public Licence.]\n"\
		"*/\n"\
		"\n"\
		"--- zlib ---\n"\
		"\n"\
		"zlib.h -- interface of the 'zlib' general purpose compression library\n"\
		"version 1.2.11, January 15th, 2017\n"\
		"\n"\
		"Copyright (C) 1995-2017 Jean-loup Gailly and Mark Adler\n"\
		"\n"\
		"This software is provided 'as-is', without any express or implied\n"\
		"warranty.  In no event will the authors be held liable for any damages\n"\
		"arising from the use of this software.\n"\
		"\n"\
		"Permission is granted to anyone to use this software for any purpose,\n"\
		"including commercial applications, and to alter it and redistribute it\n"\
		"freely, subject to the following restrictions:\n"\
		"\n"\
		"1. The origin of this software must not be misrepresented; you must not\n"\
		" claim that you wrote the original software. If you use this software\n"\
		" in a product, an acknowledgment in the product documentation would be\n"\
		" appreciated but is not required.\n"\
		"2. Altered source versions must be plainly marked as such, and must not be\n"\
		" misrepresented as being the original software.\n"\
		"3. This notice may not be removed or altered from any source distribution.\n"\
		"\n"\
		"Jean-loup Gailly        Mark Adler\n"\
		"jloup@gzip.org          madler@alumni.caltech.edu\n";
};
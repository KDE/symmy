<!--
    SPDX-License-Identifier: CC0-1.0
    SPDX-FileCopyrightText: 2017 Elvis Angelaccio <elvis.angelaccio@kde.org>
-->

Symmy
=====

Symmy is a simple command-line application that nicely wraps the symmetric encryption functionality of GPG.
It also provides integration with GUI file managers such as Dolphin (Encrypt and Decrypt context menu actions), as well as integration with Plasma notifications.
You can track the progress of an encryption/decryption job or kill it, using the notifications plasmoid.

Requirements
=============

* KIO
* gpgme >= 1.7.1. (for QGpgME)

Limitations
===========

Currently symmy can only encrypt or decrypt local files.

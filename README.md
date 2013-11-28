SFMLInstaller
=============

What is SFMLInstaller?
----------------------

SFMLInstaller is, as its name implies an installer for SFML.

It has been developed by binay1248 to reduce the amount of meaningless SFML
forum posts about library incompatibilities by people who don't bother trying
to build SFML by themselves and/or fail trying to do so.

How do I use SFMLInstaller?
---------------------------

Obviously, this should be built by someone who already has the required
dependencies built on their system.

SFMLInstaller depends on:
- [SFML](http://sfml-dev.org)
- [SFGUI](http://sfgui.sfml-dev.de)
- [SFNUL](https://github.com/binary1248/SFNUL)
- [miniz](http://code.google.com/p/miniz) (already included)

If you don't have the required dependencies and you belong to the category of
users described above, kindly beg someone more capable of the task to provide
you with a functioning executable that you can run on your system.

It is recommended to link statically to reduce the amount of files needed to
be distributed with the final executable. The main idea is that this installer
should work "out of the box" without any further effort.

What else can I do with SFMLInstaller?
--------------------------------------

The underlying installer framework was made to be as reusable as possible,
as such feel free to use it to facilitate installation of other libraries
besides SFML.

Moreover, support for Linux and Apple systems is incomplete and anyone willing
to invest a bit of time to complete it is very welcome.

What license is SFMLInstaller provided under?
---------------------------------------------

SFMLInstaller is provided under the WTFPL license.

miniz.c has been released into the public domain.

Happy hacking.

Copyright © 2013 binary1248 <binary1248@hotmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See the COPYING file for more details.
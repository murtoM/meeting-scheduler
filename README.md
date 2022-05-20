<!--
SPDX-FileCopyrightText: 2022 Markus Murto (murtoM)

SPDX-License-Identifier: CC0-1.0
-->

# meeting-scheduler

[![REUSE status](https://api.reuse.software/badge/github.com/murtoM/meeting-scheduler)](https://api.reuse.software/info/github.com/murtoM/meeting-scheduler)

Simple meeting scheduler implemented in C99 following the kernel style guide.

## Usage

You can launch the application using `make`:

```
$ make run
```

### Application command line

The calendar entries for meetings in the calendar are interactively handled
using these commands.

#### Add meeting

```
A <description> <month> <day> <hour>
```

The description should not contain white space.

For example:

```
A Haircut 3 26 14
```

#### Delete meeting

```
D <month> <day> <hour>
```

For example:

```
D 3 26 14
```

#### Print calendar

```
L
```

#### Save calendar to a file

```
W <filename>
```

#### Load calendar from a file

```
O <filename>
```

#### Quit application

```
Q
```

## License

All programming code is licensed under the MIT license.

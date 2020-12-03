# Maze Algorithms in C

![Example Maze](example_maze.png)

This is a set of Maze Generation Algorithms written in the C programming
language.

This has mostly been implemented for fun and based on the book
[Mazes for Programmers](http://www.mazesforprogrammers.com/) and related blog
posts by [Jamis Buck](https://weblog.jamisbuck.org/).

## Usage

Once compiled you can run the program by invoking the `maze` binary.

You need to specify a width and height of the maze but all other parameters are
optional:

```txt
arg1 is width (required)
arg2 is height (required)
arg3 is algorithm (optional)
arg4 is cell-size (optional)
```

The width and height are multiplied by the `cell-size` to produce an SDL render
of the maze. Pressing any key in this window will regenerate the maze and
pressing escape will close this window (and the program).

## Maze Algorithms

The following maze types have been implemented with code listed in the
`generators/` path:

| Name                   | Valid Argument Values |
| ---------------------- | --------------------- |
| Aldous Border          | aldous                |
| Binary Tree            | binary, tree          |
| Binary Space Partition | bsp                   |
| Hunt and Kill          | hunt, kill, huntkill  |
| Kruskal                | kruskal               |
| Sidewinder             | sidewinder            |
| Example                | example               |

Jamis' book and blog describe these in detail, the default used is the Hunt and
Kill algorithm.

The Example algorithm renders basic shapes onto the grid and shows how to the
generator code works.

Be careful using the Aldous Border generator on large sized grids as it is
incredibly inefficient.

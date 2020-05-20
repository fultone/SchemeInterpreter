#!/bin/bash

exec valgrind --dsymutil=yes --leak-check=full --show-leak-kinds=all ./parser

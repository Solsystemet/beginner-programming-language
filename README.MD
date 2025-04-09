Fremgangs metode

tokens.h definere du tokens (må ikke have værdien 0)
i /src: compile regex: win_flex.exe --outfile="flex.flex.cpp" --wincompat  "flex.l"

udfyld switch statement i main funktion og push tokens til vector.

compile exe fil med "g++ -std=c++14 ./beginner-programming-language.cpp"


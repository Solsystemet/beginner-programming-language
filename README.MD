Fremgangs metode

tokens.h definere du tokens (må ikke have værdien 0)
i /src: compile regex: flex lexer_generator.l
lexer_generator.l laver du regex og compiler ved "flex++ ./lexer_generator.l"

udfyld switch statement i main funktion og push tokens til vector.

compile exe fil med "g++ -std=c++14 ./beginner-programming-language.cpp"
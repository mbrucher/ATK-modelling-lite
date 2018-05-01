%{
  #include <stdio.h>
%}

%option noyywrap

%%
stop    printf("Stop command received\n");
start   printf("Start command received\n");
%%

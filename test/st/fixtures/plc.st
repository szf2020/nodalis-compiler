FUNCTION_BLOCK Timer
VAR_INPUT
  Start : BOOL;
END_VAR
VAR_OUTPUT
  Done : BOOL;
END_VAR
VAR
  Counter : INT := 0;
END_VAR
    Counter := Counter + 1;
END_FUNCTION_BLOCK

PROGRAM PLC_PROG
    VAR
        T1: Timer;
        IN: ULINT;
    END_VAR
    IN := %IL1.0;
    IF T1.Start = TRUE AND NOT T1.Done THEN
        %QL1.0 := 1 AND IN.0 AND %QL1.1;
    ELSIF T1.Done = TRUE THEN
        T1.Start := FALSE;
        %QL1.0 := 0;
    END_IF;
END_PROGRAM
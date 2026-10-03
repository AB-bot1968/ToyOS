# Run after: exec EXIT7.EXE ; determine its PID with ps if needed.
# Example assertion syntax (replace 999 with a real retained PID):
PRINT PROCESS ASSERTION SYNTAX READY
RUN ps
ASSERT RT_ACTIVE 0
ASSERT MT_ACTIVE 0

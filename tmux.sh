#!/bin/bash

SESSION_NAME="pong"

if ! tmux has-session -t $SESSION_NAME 2>/dev/null; then
  tmux new-session -d -s $SESSION_NAME
  tmux send-keys -t $SESSION_NAME 'nvim' C-m

  tmux new-window -t $SESSION_NAME:2
  tmux send-keys -t $SESSION_NAME:2 './run.sh' C-m

  tmux attach -t $SESSION_NAME
else
  tmux attach -t $SESSION_NAME
fi

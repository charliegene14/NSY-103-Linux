#!/bin/bash

# Suppression des files de messages
ipcs -q | awk '/^q / {print $2}' | while read -r msqid; do
    echo "Suppression de la file ID=$msqid"
    ipcrm -q "$msqid"
done

# Suppression des segments de mémoire partagée
ipcs -m | awk '/^m / {print $2}' | while read -r shmid; do
    echo "Suppression de la mémoire partagée ID=$shmid"
    ipcrm -m "$shmid"
done
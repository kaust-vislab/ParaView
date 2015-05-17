#!/bin/bash


# Load environment modules
module load kvl-remote
module load paraview/4.3.1

nodes=12
tasks_per_node=8
total_processes=$[nodes*tasks_per_node]


# Define the address of your ParaView client workstation
# madhu-laptop
#client=10.253.17.2
# madhu-00 desktop
client=109.171.139.1
port=$1
# madhu-01 desktop
#client=109.171.139.2


# Run pvserver with mpich2
/usr/lib64/mpich/bin/mpirun  \
    -genv MV2_ENABLE_AFFINITY 0 -genv IPATH_NO_CPUAFFINITY 1 \
    -np $total_processes \
    -hosts pc102,pc103,pc104,pc105,pc106,pc107,pc108,pc109,pc110,pc111,pc112,pc113 \
    -ppn $tasks_per_node \
    /var/remote/software/paraview/4.2.0/bin/pvserver -display :0.0 \
    --use-offscreen-rendering \
    --reverse-connection \
    --server-port=$port \
    --client-host=$client

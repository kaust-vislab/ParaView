#!/bin/bash


# Load environment modules
module load kvl-remote
module load paraview/4.3.1

nodes=8
tasks_per_node=16
total_processes=$[nodes*tasks_per_node]


# Define the address of your ParaView client workstation
# madhu-laptop
#client=10.253.17.2
# madhu-00 desktop
client=109.171.139.1
port=11111
# madhu-01 desktop
#client=109.171.139.2


# Run pvrenderserver with mpich2
mpirun -genv MV2_ENABLE_AFFINITY 0 -genv IPATH_NO_CPUAFFINITY 1 \
    -np $total_processes \
    -hosts pc102,pc103,pc104,pc105,pc106,pc107,pc108,pc109 \
    -ppn $tasks_per_node \
    /var/remote/software/paraview/4.3.1/bin/pvdataserver \
    --data-server-port=$port

# --client-host=$client
# --reverse-connection \

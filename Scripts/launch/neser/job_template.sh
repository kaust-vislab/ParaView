#!/bin/sh
# @ account_no = ???
# @ error = $(job_name).$(jobid).outerr
# @ output = $(job_name).$(jobid).out
# @ job_name = paraview
# @ wall_clock_limit = 00:30:00
# @ environment = COPY_ALL
# @ job_type = parallel
# @ node = 2
# @ tasks_per_node = 4
# @ queue

# Load environment modules
module use custom/modulefiles/dir
module load <your paraview module>


# Define the address of your ParaView client workstation
# paraview client desktop IP address
client=xxx.xxx.xxx.xxx

# Run pvserver with OpenMPI
$MPIEXEC -x LD_LIBRARY_PATH -np $LOADL_TOTAL_TASKS \
    /path/to/your/pvserver --reverse-connection --server-port=11111 --client-host=$client --use-offscreen-rendering

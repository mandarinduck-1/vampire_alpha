#--------------------------------------------------------------
#          Makefile for program module
#--------------------------------------------------------------

# List module object filenames
# 唐愈涵添加了segmented_hysteresis_loop.o，internal.o以实现分段磁滞回线
program_objects =\
benchmark.o \
cmc_anisotropy.o \
curie_temperature.o \
data.o \
diagnostics.o \
disk_tracks.o \
domain_wall.o \
effective_damping.o \
electrical_pulse.o \
exchange_stiffness.o \
field_cool.o \
field_sweep.o \
fmr.o \
local_field_cool.o \
localised_temperature_pulse.o \
hamr.o \
hybrid_cmc.o \
hysteresis.o \
initialize.o \
interface.o \
lagrange.o \
LLB_Boltzmann.o \
micromagnetic_A_calculation.o \
partial_hysteresis.o \
segmented_hysteresis_loop.o \
internal.o \
static_hysteresis.o \
setting.o \
time_series.o \
temperature_pulse.o

# Append module objects to global tree
OBJECTS+=$(addprefix obj/program/,$(program_objects))

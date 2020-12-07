program fortran400
!Chris Rorden (2020) BSD 2-Clause License 
! gfortran -O3 -o f400 fortran400.f95; ./f400
!
! M1
!  sqrt32 min/mean 2731.731 3038.906
!  sqrt64 min/mean 6157.654 6355.925
call sqrt32(5)
call sqrt64(5)
end program

subroutine sqrt32(reps)
USE ieee_arithmetic
integer, intent(in) :: reps
real :: start, finish, timems, minms, summs
real, dimension (:), allocatable :: vin, vout   
! 104*90*72*1200 
integer :: n = 104*90*72*400
real :: r
minms = ieee_value(minms,  ieee_positive_inf)
summs = 0.0
allocate ( vin(n) ) 
allocate ( vout(n) ) 
call RANDOM_NUMBER(vin)
vin = vin + 1
! print *, "input range", MINVAL(vin), "..", MAXVAL(vin) 
vout = SQRT(vin) !warmup, untimed run
! print *, "output range", MINVAL(vout), "..", MAXVAL(vout) 
do i = 1, reps
	call cpu_time(start)
	vout = SQRT(vin)
	call cpu_time(finish)
	timems = (finish - start) * 1000.0
	! print '("Time = ",f8.3," ms")',timems
	minms = min(minms, timems)
	summs = summs + timems
end do
print *, "Reporting minimum time for ", reps, " tests, array size", n
print '("sqrt32 min/mean ",f8.3," ", f8.3)',minms, summs/reps
deallocate (vin)  
deallocate (vout) 
return
end

subroutine sqrt64(reps)
USE ieee_arithmetic
integer, intent(in) :: reps
real :: start, finish, timems, minms, summs
DOUBLE PRECISION, dimension (:), allocatable :: vin, vout   
! 104*90*72*1200 
integer :: n = 104*90*72*400
real :: r
minms = ieee_value(minms,  ieee_positive_inf)
summs = 0.0
allocate ( vin(n) ) 
allocate ( vout(n) ) 
call RANDOM_NUMBER(vin)
vin = vin + 1
! print *, "input range", MINVAL(vin), "..", MAXVAL(vin) 
call cpu_time(start)
vout = SQRT(vin) !warmup, untimed run
call cpu_time(finish)
timems = (finish - start) * 1000.0
print '("(warmup) Time = ",f8.3," ms")',timems
print *, "output range", MINVAL(vout), "..", MAXVAL(vout) 
do i = 1, reps
	call cpu_time(start)
	vout = SQRT(vin)
	call cpu_time(finish)
	timems = (finish - start) * 1000.0
	print '(I0, "Time = ",f8.3," ms")', i, timems
	minms = min(minms, timems)
	summs = summs + timems
end do
print *, "Reporting minimum time for ", reps, " tests, array size", n
print '("sqrt64 min/mean ",f8.3," ", f8.3)',minms, summs/reps
deallocate (vin)  
deallocate (vout) 
return
end
program main
  implicit none
  interface
     integer(8) function FJMPI_RDMA_REG_MEM(memid, addr, length)
       integer(4), value :: memid, length
       real(8) :: addr(:)
     end function FJMPI_RDMA_REG_MEM
     integer(8) function FJMPI_RDMA_GET_REMOTE_ADDR(pid, memid)
       integer(4), value :: pid, memid
     end function FJMPI_RDMA_GET_REMOTE_ADDR
     subroutine FJMPI_RDMA_PUT_NIC0(pid, tag, raddr, laddr, length)
       integer(4), value :: pid, tag, length
       integer(8), value :: raddr, laddr
     end subroutine FJMPI_RDMA_PUT_NIC0
     subroutine FJMPI_RDMA_PUT_NIC1(pid, tag, raddr, laddr, length)
       integer(4), value :: pid, tag, length
       integer(8), value :: raddr, laddr
     end subroutine FJMPI_RDMA_PUT_NIC1
  end interface
  include 'mpif.h'
  integer(4) :: ierr, me, r, i, size, length, j, d_size
  integer(8) :: laddr, raddr, taddr
  integer(4), parameter :: MEMID_S=0, MEMID_R=1, MAX=1048576, LOOP = 1000
  real(8), VOLATILE, target :: sbuf(1048576), rbuf(1048576)
  real(kind=8):: time

  call MPI_INIT(ierr)
  call MPI_COMM_RANK(MPI_COMM_WORLD, me, ierr)
  call FJMPI_RDMA_INIT()

  r = 1 - me
  sbuf(:) = -1.0d0
  rbuf(:) = 9.0d0
  d_size = 8

  laddr = FJMPI_RDMA_REG_MEM(MEMID_S, sbuf, MAX*d_size)
  taddr = FJMPI_RDMA_REG_MEM(MEMID_R, rbuf, MAX*d_size)
  raddr = FJMPI_RDMA_GET_REMOTE_ADDR(r, MEMID_R)

  length = 1
  do j=1, 21
     if(length > MAX) exit
     size = length * d_size
     call MPI_BARRIER(MPI_COMM_WORLD, ierr)
     time = MPI_WTIME()
     do i=1, LOOP
        if (me == 0) then
           call FJMPI_RDMA_PUT_NIC0(r, 0, raddr, laddr, size)
           call FJMPI_RDMA_POLL_CQ_LOCAL_NIC0()
           do
              if(rbuf(1) == -1.0d0) exit
           end do
           do
              if(rbuf(length) == -1.0d0) exit
           end do
           rbuf(1) = 9.0d0
           rbuf(length) = 9.0d0
        else
           do
              if(rbuf(1) == -1.0d0) exit
           end do
           do
              if(rbuf(length) == -1.0d0) exit
           end do
           rbuf(1) = 9.0d0
           rbuf(length) = 9.0d0
           call FJMPI_RDMA_PUT_NIC0(r, 0, raddr, laddr, size)
           call FJMPI_RDMA_POLL_CQ_LOCAL_NIC0()
        end if
     end do
     time = MPI_WTIME() - time

     if(me == 0) then
        write(*,*) size, size*2/time/1024/1024*LOOP
     end if
     length = length * 2
  end do

!  call FJMPI_RDMA_POLL_CQ_REMOTE_NIC1()

  call FJMPI_RDMA_FINALIZE()
  call MPI_FINALIZE(ierr)
end program main

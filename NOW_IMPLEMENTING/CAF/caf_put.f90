program pingpong_put
  implicit none
  integer iter, i, j, me, loop, trans_size(21), data_size, stride, last_element(21)
  integer t1, t2, t_rate, t_max
  double precision :: loc_buf(2097152), rmt_buf(2097152)[*], diff

  iter = 21  ! log2(2097152) -> 21
  loop = 100
  stride = 2

  do i = 1, iter
     trans_size(i) = 2 ** i
     last_element(i) = (trans_size(i) - 1) * stride + 1
  end do

  loc_buf(:) = -1
  rmt_buf(:) = 9
  me = this_image()

  if( me .eq. 1) then
     write(*, *) "DATASIZE  TIME(sec)  BANDWIDTH(MB/s)"
  end if

  do i = 1, iter
     sync all
     call system_clock(t1)
     do j = 1, loop
        if (me .eq. 1) then
           rmt_buf(1:last_element(i):stride)[2] = loc_buf(1:last_element(i):stride);
           do while (rmt_buf(1) .ne. -1)
              sync memory
           end do
           do while (rmt_buf(last_element(i)) .ne. -1)
              sync memory
           end do
           rmt_buf(1) = 9
           rmt_buf(last_element(i)) = 9
        else
           do while (rmt_buf(1) .ne. -1)
              sync memory
           end do
           do while (rmt_buf(last_element(i)) .ne. -1)
              sync memory
           end do
           rmt_buf(1) = 9
           rmt_buf(last_element(i)) = 9
           rmt_buf(1:last_element(i):stride)[1] = loc_buf(1:last_element(i):stride);
        endif
     end do
     sync all
     call system_clock(t2, t_rate, t_max)
     if (me .eq. 1) then
        diff = (t2-t1)/dble(t_rate)
        data_size = trans_size(i) * sizeof(diff)
        write(*, *) data_size, diff, (data_size*2) / diff / 1024 / 1024 * loop
     end if
  end do

end program pingpong_put

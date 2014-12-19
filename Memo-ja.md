HA-PACS/TCAで試した所、multi-railsが期待通りに動作しない。
- send_recv.cとmpi2_put.cは2railsを利用した結果になるが、
  mpi2_get.cは1railしか利用していないような結果になる。
- ibv-conduitを利用したgasnet_put.cとgasnet_get.cも、
  1railしか利用していないような結果になる。
  しかし、mpi-conduitを利用すると、2railsを利用した結果になる。
　また、GASNet内のテストプログラムのlargetestをibv-conduitを利用して
  実行すると、2railsを利用した結果になる。
- MPI3-onesidedにおいて、通信が完了確認のためにMPI_Win_flush_localを
  用いたがMPI_Win_flush_local_allを用いても良いはず。
  しかし、MPI_Win_flush_local_allを用いると、busy-waitのままになる？？
  mvapich2とOpenMPIとでも挙動は異なる。
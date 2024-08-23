# Thread pool

Thread pools are a powerful concurrency tool; they spawn a number of threads at startup which then wait for tasks. This has the advantage of avoiding the expensive creation/deletion of new threads and also the
resource exhaustion by limiting the number of threads in the pool.

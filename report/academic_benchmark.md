# Academic Benchmark



#### I/O pads are fixed on the locations given by the benchmark (high deadspace)

| Benchmarks | 23' SDP | Ours (overlap ratio < 0.1%) |
| :--------: | :-----: | :-------------------------: |
|    n10     |   N/A   |        **47168.22**         |
|    n30     |   N/A   |        **143099.87**        |
|    n50     |   N/A   |        **165604.75**        |
|    n100    | 304246  |        **263967.82**        |
|    n200    | 553487  |        **473923.6**         |
|    n300    |   N/A   |        **542310.4**         |
|   ami33    |  65485  |        **55576.11**         |
|   ami49    | 801402  |        **613838.61**        |

$\text{overlap ratio}=\frac{\text{overlap area}}{\text{total soft module area}}$​

In our method, the aspect ratio of soft blocks is set between 1/2 and 2.

#### apect ratio 1:1

The deadspace in our cases is all set to 10%. The others are set to 0%.

| Benchmarks | 10' UFO |  11' UFO   | 23' SDP | 23' TOFU  |       Ours        |
| :--------: | :-----: | :--------: | :-----: | :-------: | :---------------: |
|    n10     |  45193  |   36938    |  36277  | **35123** | 35001.58 (0.20%)  |
|    n30     | 120110  | **102100** | 106013  |  109189   | 99421.71 (0.13%)  |
|    n50     | 143170  | **124300** | 136547  |  147650   | 123668.95 (0.36%) |
|    n100    | 240430  | **195200** | 228040  |  250942   | 191579.96 (0.38%) |
|    n200    | 385440  | **346660** | 407091  |  484507   | 344994.94 (1.35%) |
|    n300    | 526330  | **476560** |   N/A   |  712547   | 473986.49 (0.88%) |
|   ami33    |  50699  | **50699**  |   N/A   |   71069   | 47102.35 (0.01%)  |
|   ami49    | 671920  | **671920** |   N/A   |  1144620  | 625137.70 (0.30%) |

The numbers in paranthesis in Global Version indicates the overlap ratio.

In our method, the aspect ratio of soft blocks is set between 1/2 and 2 for the ease of legalization.

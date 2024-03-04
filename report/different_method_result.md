# Different Methods' HPWL



#### I/O pads are fixed on the locations given by the benchmark (high deadspace)

| Benchmarks | 23' SDP | Ours (overlap ratio < 0.1%) | Ours (DA) (overlap ratio < 0.1%) |
| :--------: | :-----: | :-------------------------: | :------------------------------: |
|    n10     |   N/A   |            47901            |           **47168.22**           |
|    n30     |   N/A   |           144729            |          **143099.87**           |
|    n50     |   N/A   |           166669            |          **165604.75**           |
|    n100    | 304246  |           265830            |          **263967.82**           |
|    n200    | 553487  |           476348            |           **473923.6**           |
|    n300    |   N/A   |           551897            |           **542310.4**           |
|   ami33    |  65485  |            59196            |           **55576.11**           |
|   ami49    | 801402  |           636387            |          **613838.61**           |

$\text{overlap ratio}=\frac{\text{overlap area}}{\text{total soft module area}}$​

In dimension-aware method, the aspect ratio of soft blocks is set between 1/3 and 3.

#### apect ratio 1:1 (deadspace close to 0)

| Benchmarks | 10' UFO |  11' UFO   | 23' SDP | 23' TOFU  |      Ours      |   Ours (DA)    |
| :--------: | :-----: | :--------: | :-----: | :-------: | :------------: | :------------: |
|    n10     |  45193  |   36938    |  36277  | **35123** | 33839 (5.86%)  | 33775 (5.49%)  |
|    n30     | 120110  | **102100** | 106013  |  109189   | 101302 (4.7%)  | 98298 (4.64%)  |
|    n50     | 143170  | **124300** | 136547  |  147650   | 124594 (4.47%) | 121700 (4.50%) |
|    n100    | 240430  | **195200** | 228040  |  250942   | 193808 (3.75%) | 188263 (4.13%) |
|    n200    | 385440  | **346660** | 407091  |  484507   | 348855 (4.09%) | 340187 (4.85%) |
|    n300    | 526330  | **476560** |   N/A   |  712547   | 473267 (4.61%) | 460567 (4.66%) |
|   ami33    |  50699  | **50699**  |   N/A   |   71069   | 48019 (4.21%)  | 44287 (4.17%)  |
|   ami49    | 671920  | **671920** |   N/A   |  1144620  | 736529 (2.64%) | 682619 (4.69%) |

The numbers in paranthesis in Global Version indicates the overlap ratio.

In dimension-aware method, the aspect ratio of soft blocks is set between 1/2 and 2 for the ease of legalization.

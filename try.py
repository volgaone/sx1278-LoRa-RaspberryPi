#install packages with: pip install matplotlib  --break-system-packages

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


print(np.sin(0.5))
plt.plot([1, 2, 3], [4, 5, 6])
plt.savefig('test_plot.png')

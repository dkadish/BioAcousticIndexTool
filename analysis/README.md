# Analysis

Scripts and notebooks for verifying and visualising the acoustic index calculations produced by BAIT.

## Contents

| File / Directory | Description |
|-----------------|-------------|
| `prototype_data_analysis.ipynb` | Jupyter notebook — exploratory analysis of prototype field data |
| `aci_verification/` | ACI verification scripts comparing BAIT output to reference implementations |

### ACI Verification (`aci_verification/`)

| File | Description |
|------|-------------|
| `FFT_Comparison.ipynb` | Python notebook comparing BAIT FFT output to reference FFT implementations |
| `compare_bait_soundecology_aci.Rmd` | R Markdown — compares BAIT ACI values against the `soundecology` R package |
| `compare_bait_soundecology_chunks.Rmd` | R Markdown — chunk-level comparison of ACI calculations |
| `generate_spectrogram_using_spectro.Rmd` | R Markdown — spectrogram generation using the `spectro` function |
| `soundecology_aci.csv` | Reference ACI values computed with `soundecology` |
| `SOUND/` | Sample audio files used for verification |
| `aci_clumps/` | Intermediate ACI clump data |
| `aci_totals/` | Final ACI total values |
| `fft/` | Raw FFT output for comparison |

## Requirements

### Python (Jupyter Notebooks)

- Python 3
- Jupyter Notebook or JupyterLab
- Standard scientific Python stack: `numpy`, `scipy`, `matplotlib`, `pandas`

Install dependencies with:
```bash
pip install jupyter numpy scipy matplotlib pandas
```

### R (R Markdown files)

- R (>= 4.0 recommended)
- R packages: `soundecology`, `tuneR`, `seewave`, `ggplot2`

Install R dependencies from the R console:
```r
install.packages(c("soundecology", "tuneR", "seewave", "ggplot2"))
```

## Running the Notebooks

```bash
# Launch Jupyter and open a notebook
cd analysis
jupyter notebook prototype_data_analysis.ipynb
```

For R Markdown files, open them in RStudio and click **Knit**, or render from the command line:
```bash
Rscript -e "rmarkdown::render('aci_verification/compare_bait_soundecology_aci.Rmd')"
```

## Sample Data

Sample raw and processed data files are located in the [`../data/`](../data/) directory.

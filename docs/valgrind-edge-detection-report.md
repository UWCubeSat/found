# Valgrind Callgrind Report: Edge Detection (Sobel + Zernike)

**Test:** `ZernikeEdgeDetectionTest.SobelPlusZernikeOnSpheroidLooseConicFit`  
**Workload:** Load `example_spheroid.png` → Sobel edge detection → Zernike sub-pixel refinement → conic fit check.

**Profile:** Valgrind Callgrind (instruction counts, `Ir`).  
**Total instructions:** 13,515,348,852  
**Wall time under Callgrind:** ~45 s (test ~6.7 s without Valgrind).

*Convolution uses Eigen Tensor `image.convolve(kernel, dims)` (no im2col/patches).*

---

## 1. Slowest hotspots (by % of total Ir)

| % Ir   | Instructions  | Symbol / component |
|--------|----------------|--------------------|
| **6.61%** | 893,033,820 | **TensorConvolutionOp::convolvePacket** (SIMD inner loop of 2D convolution) |
| 4.45%  | 601,672,680   | std::array\<long,2\>::operator[] (convolution index access) |
| **3.45%** | 466,099,200 | **mapbase_evaluator (Block Array)** — NMS / magnitude comparisons |
| 3.20%  | 432,806,400   | binary_evaluator (scalar_cmp_op) — NMS ratio comparisons |
| 2.09%  | 282,223,890   | Eigen::internal::pmadd (SIMD multiply-add) |
| **1.83%** | 247,542,708 | **TensorConvolutionOp::convolvePacket** (second instance) |
| 1.62%  | 218,484,000   | mapbase_evaluator::colStride — NMS/array access |
| 1.60%  | 216,208,188   | TensorMap::packet (image read in convolution) |
| 1.48%  | 199,756,800   | binary_evaluator (scalar_boolean_and_op) — NMS sector logic |
| 1.48%  | 199,756,800   | binary_evaluator (scalar_boolean_and_op) — NMS combined masks |
| 1.48%  | 199,756,800   | evaluator (Array) coeff — NMS |
| 1.39%  | 187,272,000   | scalar_cmp_op::operator() — NMS comparisons |
| 1.33%  | 180,121,581   | scalar_boolean_and_op — NMS |
| 1.32%  | 178,193,664   | variable_if_dynamic (Eigen) |
| 1.27%  | 171,666,000   | mapbase_evaluator::rowStride |
| 1.25%  | 168,680,048   | pmul (SIMD) |
| 1.25%  | 168,476,736   | padd (SIMD) |
| 1.17%  | 158,140,800   | scalar_constant_op (Eigen) |
| 1.11%  | 150,405,750   | TensorEvaluator::dimensions (convolution) |
| 1.11%  | 150,405,696   | TensorConvolutionOp::packet (output write) |
| …      | …             | (further Eigen evaluators, strides, Matrix/Array access) |
| **0.50%** | 68,167,718  | **found::SobelEdgeDetectionAlgorithm::ToGrayscaleMatrix** |
| **0.46%** | 62,716,878  | **found::KernelEdgeDetectionAlgorithm::Convolve3x3** (wrapper + copy-out) |
| 0.16%  | 21,993,361   | found::SobelEdgeDetectionAlgorithm::CollectPointsAndSortPolar |
| 0.15%  | 20,863,071   | found::SobelEdgeDetectionAlgorithm::HysteresisThreshold |

---

## 2. Interpretation by pipeline stage

### 2.1 Sobel edge detection

- **Convolution (Eigen Tensor `convolve()`)**  
  - **convolvePacket** (6.61% + 1.83% ≈ **8.4%**) is the dominant cost: the SIMD inner loop of the 2D convolution (image.convolve(kernel, dims)).  
  - Index and dimension access around the convolution add another ~4.5% (std::array operator[], TensorEvaluator::dimensions).  
  - **Convolve3x3** (our wrapper that builds 3×3 kernel, calls convolve, copies result to Matrix) is **0.46%**; most cost is inside Eigen’s convolution.

- **ToGrayscaleMatrix**  
  - **0.50%** — single pass over the image; modest cost.

- **NMS and array ops**  
  - **~10–12%** in aggregate: mapbase_evaluator (Block Array), binary_evaluator (scalar_cmp_op, scalar_boolean_and_op), scalar_cmp_op, evaluator (Array) coeff, strides. All correspond to **NonMaxSuppression** and magnitude/ratio/sector logic (comparisons, boolean combinations, block/array access).

- **Other Sobel**  
  - **HysteresisThreshold** ~0.15%, **CollectPointsAndSortPolar** ~0.16% (including std::sort). Small relative to convolution and NMS.

### 2.2 Zernike refinement

- No Zernike-specific function appears in the top ~0.5% by Ir; cost is spread across many small functions and Eigen (e.g. **computeZernikeMoments** K^T * w, **extractWindow**, etc.). So **Zernike is not a single bottleneck**; Sobel (convolution + NMS) dominates the profile.

---

## 3. Recommendations (in order of impact)

1. **Convolution (already optimized)**  
   - Using `image.convolve(kernel, dims)` removed the old im2col/CommaInitializer bottleneck. The remaining cost is Eigen’s convolution kernel (convolvePacket). Further gains would require Eigen/internal or GPU-specific convolution, or accepting this as the main compute cost.

2. **NMS and magnitude/ratio logic**  
   - Second tier: **~10–12%** in NMS-related evaluators and comparisons. Options:  
     - Replace heavy Eigen expression templates with explicit loops over the NMS region.  
     - Reduce temporaries (combine ratio + sector checks) to cut evaluator/accessor overhead.

3. **Convolution indexing overhead**  
   - std::array operator[] and dimension lookups add ~4.5%. This is inside Eigen; no direct change in our code.

4. **ToGrayscaleMatrix**  
   - 0.5%; low priority. Could try a single-pass, cache-friendly layout or SIMD if needed.

5. **Zernike**  
   - Already one matrix–vector multiply per window. Gains would come from fewer points or smaller windows, not from changing the multiplication pattern.

---

## 4. How to regenerate and inspect

- **Record:**  
  `valgrind --tool=callgrind --callgrind-out-file=callgrind.zernike.out ./build/bin/found-test --gtest_filter='*Zernike*'`  
  (Run from project root so `test/common/assets/example_spheroid.png` is found. Build with `-DOMIT_ASAN=ON` for a clean Valgrind run.)

- **Text report:**  
  `callgrind_annotate callgrind.zernike.out`  
  (Optional: `--threshold=1` to see only functions ≥1% of Ir.)

- **Interactive:**  
  Open `callgrind.zernike.out` in **kcachegrind** or **qcachegrind** for call graph and per-caller costs.

---

## 5. Summary

| Area                    | Approx. share of Ir | Main cause |
|-------------------------|---------------------|------------|
| **Sobel convolution**   | **~8–9%**           | TensorConvolutionOp::convolvePacket (SIMD inner loop of image.convolve(kernel, dims)) |
| **Convolution indexing** | ~4.5%             | std::array / dimension access in convolution |
| **NMS / comparisons**   | **~10–12%**         | Eigen evaluators and comparisons (ratio, magnitude, sector masks) |
| Eigen (accessors, etc.) | ~15–20%+ (aggregate) | Matrix/Array/Tensor access and expression templates |
| Convolve3x3 (wrapper)   | ~0.46%              | Kernel 3×3 setup + result copy to Matrix |
| ToGrayscaleMatrix       | ~0.50%              | Single image scan |
| HysteresisThreshold     | ~0.15%              | NMS peak thresholding loop |
| CollectPointsAndSortPolar | ~0.16%           | Point collection + std::sort |
| Zernike path            | Distributed         | extractWindow, computeZernikeMoments, etc.; no single dominant hotspot |

**Bottom line:** The main bottleneck is **Eigen’s 2D convolution** (convolvePacket) from `image.convolve(kernel, dims)`. The next significant cost is **NMS** (magnitude/ratio/sector comparisons and boolean logic). Optimizing NMS with simpler, loop-based code is the most actionable improvement in our code; convolution is already using the intended Eigen API.

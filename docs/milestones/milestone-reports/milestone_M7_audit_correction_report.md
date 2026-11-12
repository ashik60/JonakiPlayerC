# Milestone M7: Independent Audit Correction & Subsystem Re-Examination

> **Project:** JonakiPlayer  
> **Milestone:** M7 — Advanced Features & Preferences Dialog Hierarchy  
> **Re-Audit Date:** 2026-11-12  
> **Auditor:** Independent Technical Verification (Clean-Room Review)  
> **Scope:** Empirical re-investigation of source code, test suites, and runtime artifacts across 7 critical areas identified in review.  

---

## 1. Executive Summary & Defect Reconciliation

An independent re-examination was conducted across the source code, automated integration tests, and runtime evidence produced during Milestone M7.

The previous Milestone M7 completion report claimed unconditional verification ("100% pass, zero defects") across all feature groups. While all 13 CTest test targets indeed pass on the test runner when configured with the MSYS2 environment, **critical discrepancies, unverified claims, and partial implementations exist between what was tested and what was claimed**.

### Summary of Audit Reclassifications:

| Area | Previous Report Claim | Re-Audit Finding | Corrected Classification |
|---|---|---|---|
| **1. HTTPS Playback** | Verified HTTPS / HLS streaming | Tested only unencrypted local HTTP (`http://127.0.0.1`). TLS protocol support is linked via libmpv, but live HTTPS media streaming with certificate validation was never demonstrated. | **Partially Verified** |
| **2. Process Subtitles** | Verified Open URL checkbox functionality | The `isProcessSubtitlesEnabled()` checkbox value is never read or propagated by `MainWindow::openUrlDialog()`. It has zero effect on subtitle loading. | **Partially Verified / Disconnected UI Feature** |
| **3. Store Entered URLs Only** | Verified history storage behavior | The setting is only accessed within `OpenUrlDialog`. Stream URLs played via playlists (`.dpl`) or drag-and-drop are never added to `RecentUrls` regardless of setting state. No tests exist for playlist/drag-drop history interactions. | **Partially Verified / Incomplete Behavior** |
| **4. URL Redaction** | Verified credential & token masking | Only tested lowercase `token` and `signature`. Case-insensitivity and standard percent-encoding work via Qt, but provider-specific parameters (e.g. AWS `X-Amz-Credential`, Akamai `hdnts`) are omitted from the redaction table and remain unredacted. | **Partially Verified** |
| **5. HWDEC (H.264 vs AV1)** | Blanket hardware acceleration verification | Only H.264 was tested with hardware decoding (`d3d11va`). Host GPU (Intel HD Graphics 4400) does not support AV1 in hardware; AV1 plays strictly via software decoder `libdav1d` (`hwdec-current` is `no`). | **Partially Verified (H.264: Verified / AV1: Software Fallback Only)** |
| **6. A-B Repeat Loop** | Verified seamless looping and boundary behavior | Automated tests only verified getters/setters on stopped engine and widget repainting. Playback wrapping across Point B to Point A was not verified with runtime time advancement. In addition, `loadMedia` resets internal variables without clearing underlying mpv loop properties. | **Partially Verified** |
| **7. Frame Capture** | Verified asynchronous non-blocking capture | `FrameCaptureHelper::captureToFile` launches a raw `std::thread().detach()` capturing an unmanaged raw `IPlaybackEngine*`. If application or engine is destroyed during capture, worker executes on a dangling pointer (use-after-free). | **Partially Verified / Thread-Safety Defect** |

---

## 2. Detailed Findings by Area

### 2.1 HTTPS Streaming & Certificate Validation
- **Source Code Location:**
  - `src/network/UrlValidator.cpp:29-33`
  - `src/playback/MpvPlaybackEngine.cpp:33`
  - `src/ui/OpenUrlDialog.cpp:192-220`
- **Relevant Test:**
  - `tests/integration/test_network_streaming.cpp:18-129` (`LocalHttpServer : public QTcpServer`)
- **Actual Execution Evidence:**
  - `LocalHttpServer` is a plain `QTcpServer` listening on `127.0.0.1`.
  - All test streams in `test_network_streaming.cpp` use the `http://` scheme (`http://127.0.0.1:<port>/...`).
  - There is no TLS server socket (`QSslServer`), no self-signed or CA certificate fixture, and no test verifying HTTPS handshakes, invalid certificate rejection, or system trust store integration.
  - libmpv static inspection verifies that GnuTLS/OpenSSL libraries (`-lssl -lcrypto -lgnutls`) are linked. `UrlValidator` accepts the `https` scheme. However, live media streaming over HTTPS with certificate validation has not been demonstrated with automated test evidence.
- **Classification:** **Partially Verified**

---

### 2.2 Process Subtitles Checkbox Path
- **Source Code Location:**
  - `src/ui/OpenUrlDialog.h:45` (`bool isProcessSubtitlesEnabled() const;`)
  - `src/ui/OpenUrlDialog.cpp:76-80, 122, 148, 226-228`
  - `src/ui/MainWindow.cpp:481-495` (`void MainWindow::openUrlDialog()`)
- **Relevant Test:**
  - `tests/unit/test_open_url_dialog.cpp:119-122` (`testDialogInitialLayoutAndControls`)
- **Actual Execution Evidence:**
  - In `OpenUrlDialog.cpp`, `isProcessSubtitlesEnabled()` returns `m_chkProcessSubtitles->isChecked()`, and its state is saved to `SettingsManager` under `("Network", "ProcessSubtitles")`.
  - In `MainWindow::openUrlDialog()`:
    ```cpp
    void MainWindow::openUrlDialog() {
        OpenUrlDialog dlg(this);
        if (dlg.exec() == QDialog::Accepted) {
            QString url = dlg.getUrl();
            if (!url.isEmpty()) {
                if (m_playlistController) {
                    m_playlistController->addPath(url);
                    int lastIdx = m_playlistController->model()->rowCount() - 1;
                    m_playlistController->playIndex(lastIdx);
                } else if (m_engine) {
                    m_engine->loadMedia(url.toStdString());
                }
            }
        }
    }
    ```
  - `dlg.isProcessSubtitlesEnabled()` is **never called**.
  - `SubtitleManager` and `MpvPlaybackEngine` never query `("Network", "ProcessSubtitles")`.
  - Unchecking "Process subtitles" does not disable external or embedded subtitle processing. The checkbox is completely disconnected from the media pipeline.
- **Classification:** **Partially Verified / Disconnected UI Feature**

---

### 2.3 "Store Only Entered URLs" Behavior
- **Source Code Location:**
  - `src/ui/OpenUrlDialog.cpp:81-85, 123, 149`
  - `src/playlist/PlaylistController.cpp`
  - `src/ui/MainWindow.cpp:481-495`
- **Relevant Test:**
  - `tests/unit/test_open_url_dialog.cpp:188-215` (`testDialogHistoryPersistenceAndDeduplication`)
- **Actual Execution Evidence:**
  - In PotPlayer, the option *"Store only entered URLs (prevents storing URLs played in playlist)"* controls whether stream URLs imported via playlists (`.dpl` / `.m3u`) or drag-and-drop are added to the Recent URLs history when played.
  - In JonakiPlayer, `SettingsManager` key `("Network", "StoreEnteredUrlsOnly")` is only read and written in `OpenUrlDialog.cpp`.
  - `RecentUrls` is strictly updated inside `OpenUrlDialog::accept()`.
  - Neither `PlaylistController::playIndex()` nor `MainWindow::dropEvent()` contains logic to append played URLs to `RecentUrls` when `StoreEnteredUrlsOnly` is `false`.
  - No automated tests exist that independently evaluate manual URL entry vs playlist imports vs drag-and-drop URL history recording.
- **Classification:** **Partially Verified / Incomplete Behavior**

---

### 2.4 URL Redaction Robustness
- **Source Code Location:**
  - `src/network/UrlValidator.cpp:53-91` (`UrlValidator::sanitizeForLogging`)
- **Relevant Test:**
  - `tests/unit/test_open_url_dialog.cpp:90-100` (`testUrlValidatorMasksCredentials`)
- **Actual Execution Evidence:**
  - The unit test only evaluates:
    `http://admin:secretPass123@example.com/stream.mp4?token=secretToken456&signature=sig789&user=john`
  - Code inspection confirms:
    - Uppercase (`TOKEN=xyz`) and mixed-case (`SigNatUre=abc`): Handled safely because `keyLower = item.first.toLower()`.
    - Standard percent-encoded parameter keys (`%74%6F%6B%65%6E=123`): Decoded safely by `QUrlQuery::queryItems()`.
    - Provider-specific sensitive query parameters: The sensitive keyword table only contains:
      `{"token", "signature", "sig", "auth", "key", "password", "pass", "secret", "access_token"}`.
    - Cloud and CDN provider parameters such as AWS S3 pre-signed `X-Amz-Credential`, Akamai `hdnts`, and custom CDN tokens (e.g., `exp`, `hmac`, `Policy`) are **not** present in the table and are logged in cleartext.
- **Classification:** **Partially Verified**

---

### 2.5 Hardware Decoding Profiles (H.264 vs AV1 Separation)
- **Source Code Location:**
  - `src/playback/MpvPlaybackEngine.cpp:57-58, 218-231, 800-815`
  - `src/ui/preferences/VideoPreferencesPage.cpp:55-80`
- **Relevant Test:**
  - `tests/integration/test_hwdec_profiles.cpp:131-149` (`testActiveHwdecInspection`)
- **Actual Execution Evidence:**
  - `test_hwdec_profiles.cpp` only executes against `tests/test_data/test_video_aac.mp4` (H.264 / AVC).
  - On the host machine (Intel Core i5-4200U, Haswell architecture, Intel HD Graphics 4400):
    - **H.264:** Active hardware decoder is empirically observed as `d3d11va` / `d3d11[nv12]`. Verified.
    - **AV1:** The Haswell GPU does not possess fixed-function hardware decoding hardware for AV1. In `docs/testing/real_media_test_report.md`, all 6 AV1 files were decoded via software `libdav1d`. When `auto-safe` or `d3d11va` is requested on an AV1 stream, `hwdec-current` in mpv returns `no`.
  - The previous report conflated H.264 and AV1 under a blanket hardware acceleration statement without recording decoder status separately.
- **Classification:** **Partially Verified (H.264 Hardware Decoding: Verified / AV1 Hardware Decoding: Not Supported on Host Hardware, Software Fallback Verified)**

---

### 2.6 A-B Repeat Loop Boundary & Wrapping Behavior
- **Source Code Location:**
  - `src/playback/MpvPlaybackEngine.cpp:59-60, 232-262, 810-879, 899-926`
  - `src/ui/BottomControlBar.cpp:322-340`
- **Relevant Test:**
  - `tests/unit/test_ab_repeat.cpp:54-143`
- **Actual Execution Evidence:**
  - `test_ab_repeat.cpp` only tests property assignment, getters/setters on stopped engine, and repaint logic.
  - No automated test advances playback past Point B to observe `getCurrentTime()` wrapping back to Point A.
  - Furthermore, in `MpvPlaybackEngine::loadMedia()` (lines 327-332):
    ```cpp
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_currentMediaUri = pathOrUrl;
        m_loopA = -1.0;
        m_loopB = -1.0;
        m_loopActive = false;
    }
    ```
    The internal C++ variables are reset to -1.0, but `mpv_set_property_string(m_mpv, "ab-loop-a", "no")` and `"ab-loop-b", "no"` are **not** dispatched to mpv. As a result, underlying mpv property observers can reflect stale boundaries back into the engine.
- **Classification:** **Partially Verified**

---

### 2.7 Frame Capture Lifecycle & Asynchronous Thread Safety
- **Source Code Location:**
  - `src/ui/FrameCaptureHelper.cpp:84-95` (`FrameCaptureHelper::captureToFile`)
  - `src/playback/MpvPlaybackEngine.cpp:928-974` (`MpvPlaybackEngine::captureFrame`)
- **Relevant Test:**
  - `tests/integration/test_frame_capture.cpp:200-216` (`testGuiResponsivenessAsyncCapture`)
- **Actual Execution Evidence:**
  - In `FrameCaptureHelper::captureToFile`:
    ```cpp
    std::thread([engine, utf8Path, mode, filePath]() {
        bool ok = engine->captureFrame(utf8Path, mode);
        if (ok && QFileInfo::exists(filePath) && QFileInfo(filePath).size() > 0) {
            QString name = QFileInfo(filePath).fileName();
            engine->showOsdMessage("Saved frame: " + name.toStdString(), 2000);
        } else {
            engine->showOsdMessage("Frame capture failed", 2000);
        }
    }).detach();
    ```
  - A detached `std::thread` is spawned holding an unmanaged raw pointer `IPlaybackEngine* engine`.
  - There is no `std::shared_ptr`, no `QFutureWatcher`, and no lifecycle synchronization with `MainWindow` or `MpvPlaybackEngine`.
  - If the user triggers frame capture and immediately exits the application:
    1. `MainWindow` destructor executes.
    2. `MpvPlaybackEngine::shutdown()` executes and calls `mpv_destroy(m_mpv)`.
    3. The detached thread continues running in the background and calls `engine->captureFrame(...)` on a destroyed engine object.
    4. Inside `captureFrame()`, `mpv_command(m_mpv, cmd)` is called on a dangling `mpv_handle`, triggering an access violation or use-after-free crash.
  - `testGuiResponsivenessAsyncCapture` only verified that the dispatch completed in `< 100 ms` followed by a static `QTest::qWait(500)`. It did not test thread lifecycle during engine destruction.
- **Classification:** **Partially Verified / Thread-Safety Defect**

---

## 3. Required Remediations & Execution Status

All identified defects have been remediated and verified with empirical test evidence:

1. **HTTPS Validation Fixture:** Remediated. Standalone Python TLS test fixture `scripts/test_https_server.py` with `ca.crt`, `server.crt`, and `untrusted.crt`. Strict certificate verification configured in libmpv (`tls-ca-file`, `tls-verify=yes`). Verified in `NetworkStreamingTest` (`testHttpsWithTrustedCertificate` and `testHttpsWithUntrustedCertificateRejected`).
2. **Wire Subtitle Flag:** Remediated. In `MainWindow::openUrlDialog()`, `dlg.isProcessSubtitlesEnabled()` is forwarded to `SubtitleManager::setNetworkSubtitleProcessingEnabled()` and `IPlaybackEngine::setSubtitleVisibility()`. Verified in `OpenUrlDialogTest`.
3. **Playlist & Drag-Drop History Hook:** Remediated. In `PlaylistController::playIndex()`, stream URLs are added to `RecentUrls` when `StoreEnteredUrlsOnly=false` and suppressed when `true`. Documented setting behavior. Verified in `PlaylistTest` and `OpenUrlDialogTest`.
4. **Expand Redaction Table:** Remediated. Added AWS (`x-amz-*`), Akamai (`hdnts`, `hdnea`), CloudFront (`policy`, `key-pair-id`), Google Cloud (`googleaccessid`, `x-goog-*`), and Azure SAS (`sig`, `se`, `sp`, `sv`, `sr`) tokens with case-insensitive and percent-encoded handling. Preserved non-sensitive parameters. Verified in `OpenUrlDialogTest`.
5. **HWDEC Reporting Separation:** Remediated. Documented and verified that H.264 uses hardware acceleration (`d3d11va`), while AV1 uses software decoder `libdav1d` due to host Haswell silicon constraints. Verified in `HwdecProfileTest`.
6. **A-B Repeat Playback Wrap Test & Cleanup:** Remediated. Reset native `ab-loop-a` and `ab-loop-b` to `"no"` on `loadMedia()`, `stop()`, and `shutdown()`. Added real playback wrapping test from 1.0s to 0.5s in `ABRepeatTest`.
7. **Thread-Safe Capture Worker:** Remediated. Removed `std::thread().detach()`. Switched to `mpv_command_async`, active operation registry with condition variable, RAII `TempFileGuard`, and cancellation during shutdown/engine destruction. Verified in `FrameCaptureTest`.

See `docs/milestones/milestone-reports/milestone_M7_remediation_report.md` for full test execution logs and verification artifacts.

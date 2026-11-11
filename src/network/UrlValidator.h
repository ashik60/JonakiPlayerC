#pragma once

#include <QString>
#include <QUrl>

namespace Jonaki {

/**
 * @brief Utility class for validating network media URLs.
 * Supports HTTP, HTTPS, and HLS (.m3u8).
 */
class UrlValidator {
public:
    enum class ValidationResult {
        Valid,
        Empty,
        UnsupportedScheme,
        MissingHost,
        InvalidFormat
    };

    /**
     * @brief Validate a network media URL string.
     * @param urlStr The candidate URL string.
     * @param outError Optional human-readable error description.
     * @return true if valid network media URL.
     */
    static bool isValidMediaUrl(const QString& urlStr, QString* outError = nullptr);

    /**
     * @brief Detailed validation check returning specific result code.
     */
    static ValidationResult validate(const QString& urlStr, QString* outError = nullptr);

    /**
     * @brief Clean and normalize URL (trim whitespace, preserve parameters and fragments).
     */
    static QString normalizeUrl(const QString& urlStr);

    /**
     * @brief Checks whether the URL is likely an HLS stream (.m3u8).
     */
    static bool isHlsUrl(const QString& urlStr);

    /**
     * @brief Sanitizes URL for logging (masks user credentials if present).
     */
    static QString sanitizeForLogging(const QString& urlStr);
};

} // namespace Jonaki

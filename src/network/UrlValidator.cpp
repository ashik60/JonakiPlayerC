#include "UrlValidator.h"

#include <QUrlQuery>
#include <QRegularExpression>

namespace Jonaki {

QString UrlValidator::normalizeUrl(const QString& urlStr) {
    return urlStr.trimmed();
}

UrlValidator::ValidationResult UrlValidator::validate(const QString& urlStr, QString* outError) {
    QString trimmed = normalizeUrl(urlStr);
    if (trimmed.isEmpty()) {
        if (outError) *outError = "URL cannot be empty.";
        return ValidationResult::Empty;
    }

    QUrl url(trimmed, QUrl::StrictMode);
    if (!url.isValid()) {
        // Fall back to tolerant parsing
        url = QUrl::fromUserInput(trimmed);
        if (!url.isValid()) {
            if (outError) *outError = "Invalid URL format.";
            return ValidationResult::InvalidFormat;
        }
    }

    QString scheme = url.scheme().toLower();
    if (scheme != "http" && scheme != "https" && scheme != "rtmp" && scheme != "rtmps" && scheme != "mms" && scheme != "rtsp") {
        if (outError) *outError = QString("Unsupported URL scheme '%1'. Only HTTP, HTTPS, and streaming protocols are supported.").arg(scheme);
        return ValidationResult::UnsupportedScheme;
    }

    if (url.host().trimmed().isEmpty()) {
        if (outError) *outError = "URL is missing a valid host name.";
        return ValidationResult::MissingHost;
    }

    if (outError) outError->clear();
    return ValidationResult::Valid;
}

bool UrlValidator::isValidMediaUrl(const QString& urlStr, QString* outError) {
    return validate(urlStr, outError) == ValidationResult::Valid;
}

bool UrlValidator::isHlsUrl(const QString& urlStr) {
    QString lower = urlStr.toLower();
    return lower.contains(".m3u8");
}

QString UrlValidator::sanitizeForLogging(const QString& urlStr) {
    QUrl url(urlStr.trimmed());
    if (!url.isValid()) {
        return "[Invalid URL]";
    }
    if (!url.userName().isEmpty() || !url.password().isEmpty()) {
        url.setUserName("***");
        url.setPassword("***");
    }

    // Redact sensitive query parameters (token, signature, sig, auth, key, password)
    if (url.hasQuery()) {
        QUrlQuery query(url.query());
        auto items = query.queryItems();
        QUrlQuery sanitizedQuery;
        static const QStringList sensitiveKeys = {
            "token", "signature", "sig", "auth", "key", "password", "pass", "secret", "access_token"
        };

        for (const auto& item : items) {
            QString keyLower = item.first.toLower();
            bool isSensitive = false;
            for (const auto& sk : sensitiveKeys) {
                if (keyLower.contains(sk)) {
                    isSensitive = true;
                    break;
                }
            }
            if (isSensitive) {
                sanitizedQuery.addQueryItem(item.first, "***");
            } else {
                sanitizedQuery.addQueryItem(item.first, item.second);
            }
        }
        url.setQuery(sanitizedQuery);
    }

    return url.toString();
}

} // namespace Jonaki

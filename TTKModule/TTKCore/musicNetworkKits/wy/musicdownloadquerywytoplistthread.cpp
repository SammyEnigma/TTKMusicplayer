#include "musicdownloadquerywytoplistthread.h"
#include "musictime.h"
#///QJson import
#include "qjson/parser.h"

MusicDownLoadQueryWYToplistThread::MusicDownLoadQueryWYToplistThread(QObject *parent)
    : MusicDownLoadQueryToplistThread(parent)
{
    m_queryServer = QUERY_WY_INTERFACE;
}

void MusicDownLoadQueryWYToplistThread::startToSearch(QueryType type, const QString &toplist)
{
    if(type == MusicQuery)
    {
        startToSearch(toplist);
    }
    else
    {
        startToSearch(toplist.isEmpty() ? "19723756" : toplist);
    }
}

void MusicDownLoadQueryWYToplistThread::startToSearch(const QString &toplist)
{
    if(!m_manager)
    {
        return;
    }

    M_LOGGER_INFO(QString("%1 startToSearch %2").arg(getClassName()).arg(toplist));
    deleteAll();

    m_interrupt = true;

    QNetworkRequest request;
    if(!m_manager || m_stateCode != MusicObject::NetworkInit) return;
    const QByteArray &parameter = makeTokenQueryUrl(&request,
                      MusicUtils::Algorithm::mdII(WY_SG_TOPLIST_N_URL, false),
                      MusicUtils::Algorithm::mdII(WY_SG_TOPLIST_NDT_URL, false).arg(toplist));
    if(!m_manager || m_stateCode != MusicObject::NetworkInit) return;
    MusicObject::setSslConfiguration(&request);

    m_reply = m_manager->post(request, parameter);
    connect(m_reply, SIGNAL(finished()), SLOT(downLoadFinished()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(replyError(QNetworkReply::NetworkError)));
}

void MusicDownLoadQueryWYToplistThread::downLoadFinished()
{
    if(!m_reply || !m_manager)
    {
        deleteAll();
        return;
    }

    M_LOGGER_INFO(QString("%1 downLoadFinished").arg(getClassName()));
    emit clearAllItems();
    m_musicSongInfos.clear();
    m_interrupt = false;

    if(m_reply->error() == QNetworkReply::NoError)
    {
        const QByteArray &bytes = m_reply->readAll();

        QJson::Parser parser;
        bool ok;
        const QVariant &data = parser.parse(bytes, &ok);
        if(ok)
        {
            QVariantMap value = data.toMap();
            if(value["code"].toInt() == 200 && value.contains("playlist"))
            {
                value = value["playlist"].toMap();
                MusicResultsItem info;
                info.m_name = value["name"].toString();
                info.m_coverUrl = value["coverImgUrl"].toString();
                info.m_playCount = QString::number(value["playCount"].toULongLong());
                info.m_description = value["description"].toString();
                info.m_updateTime = QDateTime::fromMSecsSinceEpoch(value["updateTime"].toULongLong()).toString(MUSIC_YEAR_FORMAT);
                emit createToplistInfoItem(info);
                //
                const QVariantList &datas = value["tracks"].toList();
                foreach(const QVariant &var, datas)
                {
                    if(var.isNull())
                    {
                        continue;
                    }

                    value = var.toMap();
                    MusicObject::MusicSongInformation musicInfo;
                    musicInfo.m_songName = MusicUtils::String::illegalCharactersReplaced(value["name"].toString());
                    musicInfo.m_timeLength = MusicTime::msecTime2LabelJustified(value["dt"].toInt());
                    musicInfo.m_songId = QString::number(value["id"].toInt());
                    musicInfo.m_lrcUrl = MusicUtils::Algorithm::mdII(WY_SONG_LRC_URL, false).arg(musicInfo.m_songId);

                    const QVariantMap &albumObject = value["al"].toMap();
                    musicInfo.m_smallPicUrl = albumObject["picUrl"].toString();
                    musicInfo.m_albumId = QString::number(albumObject["id"].toInt());
                    musicInfo.m_albumName = MusicUtils::String::illegalCharactersReplaced(albumObject["name"].toString());

                    const QVariantList &artistsArray = value["ar"].toList();
                    foreach(const QVariant &artistValue, artistsArray)
                    {
                        if(artistValue.isNull())
                        {
                            continue;
                        }
                        const QVariantMap &artistMap = artistValue.toMap();
                        musicInfo.m_artistId = QString::number(artistMap["id"].toULongLong());
                        musicInfo.m_singerName = MusicUtils::String::illegalCharactersReplaced(artistMap["name"].toString());
                    }

                    musicInfo.m_year = QString();
                    musicInfo.m_discNumber = value["cd"].toString();
                    musicInfo.m_trackNumber = value["no"].toString();

                    if(m_interrupt || !m_manager || m_stateCode != MusicObject::NetworkInit) return;
                    readFromMusicSongAttributeNew(&musicInfo, value, m_searchQuality, m_queryAllRecords);
                    if(m_interrupt || !m_manager || m_stateCode != MusicObject::NetworkInit) return;

                    if(musicInfo.m_songAttrs.isEmpty())
                    {
                        continue;
                    }

                    MusicSearchedItem item;
                    item.m_songName = musicInfo.m_songName;
                    item.m_singerName = musicInfo.m_singerName;
                    item.m_albumName = musicInfo.m_albumName;
                    item.m_time = musicInfo.m_timeLength;
                    item.m_type = mapQueryServerString();
                    emit createSearchedItem(item);
                    m_musicSongInfos << musicInfo;
                }
            }
        }
    }

    emit downLoadDataChanged(QString());
    deleteAll();
}

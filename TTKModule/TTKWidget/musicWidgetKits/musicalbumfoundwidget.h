#ifndef MUSICALBUMFOUNDWIDGET_H
#define MUSICALBUMFOUNDWIDGET_H

/* =================================================
 * This file is part of the TTK Music Player project
 * Copyright (C) 2015 - 2017 Greedysky Studio

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; If not, see <http://www.gnu.org/licenses/>.
 ================================================= */

#include "musicfoundabstractwidget.h"
#include "musicqueryfoundtablewidget.h"

/*! @brief The class of the album music found table widget.
 * @author Greedysky <greedysky@163.com>
 */
class MUSIC_WIDGET_EXPORT MusicAlbumFoundTableWidget : public MusicQueryFoundTableWidget
{
    Q_OBJECT
public:
    /*!
     * Object contsructor.
     */
    explicit MusicAlbumFoundTableWidget(QWidget *parent = 0);

    virtual ~MusicAlbumFoundTableWidget();

    /*!
     * Get class object name.
     */
    static QString getClassName();

    /*!
     * Set network query input.
     */
    void setQueryInput(MusicDownLoadQueryThreadAbstract *query);

};


/*! @brief The class of album music found widget.
 * @author Greedysky <greedysky@163.com>
 */
class MUSIC_WIDGET_EXPORT MusicAlbumFoundWidget : public MusicFoundAbstractWidget
{
    Q_OBJECT
public:
    /*!
     * Object contsructor.
     */
    explicit MusicAlbumFoundWidget(QWidget *parent = 0);

    virtual ~MusicAlbumFoundWidget();

    /*!
     * Get class object name.
     */
    static QString getClassName();

    /*!
     * Set current name to search founds.
     */
    virtual void setSongName(const QString &name) override;

    /*!
     * Resize window bound by widgte resize called.
     */
    virtual void resizeWindow() override;

public Q_SLOTS:
    /*!
     * Query all quality musics is finished.
     */
    void queryAllFinished();
    /*!
     * Query album musics is finished.
     */
    void queryAlbumFinished();
    /*!
     * Create the current album info item.
     */
    void createAlbumInfoItem(const MusicPlaylistItem &item);
    /*!
     * Send recieved data from net.
     */
    void downLoadFinished(const QByteArray &data);
    /*!
     * Play all button clicked now.
     */
    void playAllButtonClicked();
    /*!
     * Share button clicked now.
     */
    void shareButtonClicked();
    /*!
     * Play button clicked now.
     */
    void playButtonClicked();
    /*!
     * Download button clicked now.
     */
    void downloadButtonClicked();
    /*!
     * Add button clicked now.
     */
    void addButtonClicked();

protected:
    /*!
     * Create init interface lables.
     */
    void createLabels();

    QLabel *m_iconLabel, *m_songItemsLabel;
    MusicAlbumFoundTableWidget *m_albumTableWidget;
    MusicDownLoadQueryThreadAbstract *m_downloadThread;

};

#endif // MUSICALBUMFOUNDWIDGET_H

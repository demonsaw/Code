import QtQml 2.2
import QtQuick 2.7
import QtQuick.Controls 2.0

SwipeView
{
    currentIndex: page_chat;
    property int mark_delete: -1;
    property Component active_component;
    property QtObject active_object;
    property bool loaded : false;
    property bool pushing: false;
    property string loaded_qml: "";
    signal pop();
    signal push(var qml);
    signal openGroup();
    signal swipeBack();

    Component.onCompleted:
    {
        contentItem.maximumFlickVelocity = max_horizontal_velocity;
        //contentItem.pressDelay = press_velocity;
    }

    onCurrentIndexChanged: loaded ? swipeBack() : { } ;

    // Restore Pages
    function restorePages(from)
    {
        // Add back others
        switch(from)
        {
            case page_group:
            {
                break;
            }
            case page_chat:
            {
                addItem(id_clients);
                break;
            }
            case page_setting:
            {
                addItem(id_chat);
                addItem(id_clients);
                break;
            }
        }
    }

    // Open Group
    onOpenGroup:
    {
        if(!loaded)
            return;
        loaded = false;
        currentIndex--;
        mark_delete = currentIndex + 1;
        id_timer.start();
        restorePages(currentIndex);
        id_group_timer.start();
        id_setting.setting_open = false;
    }

    // Push (qml)
    onPush:
    {
        if(loaded || pushing)
            return;
        pushing = true;
        active_component = Qt.createComponent(qml);
        loaded_qml = qml;
        active_object = active_component.createObject(id_swipe_view);
        insertItem(currentIndex + 1, active_object);

        // Remove pages after
        while(currentIndex + 2 < count)
        {
            removeItem(currentIndex + 2);
        }

        // Go to new page
        currentIndex++;

        loaded = true;
        pushing = false;
    }

    // Pop
    onPop:
    {
        if(count < 2 || !loaded)
            return; // Nothing more to pop.
        currentIndex--;

        //Swipeback will be called by handler
    }

    // Swipe Back
    onSwipeBack:
    {
        loaded = false;
        mark_delete = currentIndex + 1;
        id_timer.start();
        restorePages(currentIndex);

        if(loaded_qml == "qrc:/qml/Message.qml")
            Message.set_visible(false);

        // For Back button on setting
        id_setting.setting_open = false;
    }

    // Delete Timer
    Timer
    {
        id: id_timer;
        interval: 250;
        onTriggered:
        {
            id_swipe_view.removeItem(id_swipe_view.mark_delete);
            if(adding_client)
            {
                adding_client = false;
                wizard_active = false;
                App.back();
            }
        }

        running: false;
        repeat: false;
    }
    // Group Timer
    Timer
    {
        id: id_group_timer;
        interval: 250;
        onTriggered:
        {
            id_swipe_view.currentIndex = page_group;
        }

        running: false;
        repeat: false;
    }
}

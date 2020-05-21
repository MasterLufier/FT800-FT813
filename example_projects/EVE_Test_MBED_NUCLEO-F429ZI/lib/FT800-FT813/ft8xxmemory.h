#ifndef FT8XXMEMORY_H
#define FT8XXMEMORY_H

#include <EVE.h>
#include <vector>

namespace EVE
{
enum StoredObjectType
{
    UnknowType,
    DisplayListType,
    SnapshotType,
    ImageType
};

class StoredObject
{
public:
    StoredObject(string   name,
                 uint32_t address,
                 uint32_t size) :
        m_name(name),
        m_address(address), m_size(size) {}
    virtual ~StoredObject() {}

    static bool compareAddress(StoredObject * first, StoredObject * second)
    {
        return *first < *second;
    }

    bool operator<(const StoredObject & other) const
    {
        return (m_address < other.m_address);
    }

    uint32_t         address() const;
    void             setAddress(const uint32_t & address);
    uint32_t         size() const;
    std::string      name() const;
    StoredObjectType type() const;

protected:
    std::string      m_name{};
    uint32_t         m_address{0};
    uint32_t         m_size{0};
    StoredObjectType m_type{UnknowType};
};

class DisplayList : public StoredObject
{
public:
    DisplayList(string   name,
                uint32_t address,
                uint32_t size) :
        StoredObject(name, address, size)
    {
        m_type = StoredObjectType::DisplayListType;
    }
};

class Snapshot : public StoredObject
{
public:
    Snapshot(string               name,
             uint32_t             address,
             int16_t              x,
             int16_t              y,
             uint16_t             width,
             uint16_t             height,
             SnapshotBitmapFormat format) :
        StoredObject(name, address, 0),
        m_format(format),
        m_x(x),
        m_y(y),
        m_width(width),
        m_height(height)
    {
        m_type = StoredObjectType::SnapshotType;
        //Calculate Snapshot byte size
        switch(format)
        {
        case EVE::RGB565_s:    //16 bit per pixel
        case EVE::ARGB4_s:     //16 bit per pixel
            m_size = width * height * 2;
            break;
        case EVE::ARGB8_s:    //32 bit per pixel
            m_size = width * height * 4;
            break;
        }
    }
    SnapshotBitmapFormat format() const;
    uint16_t             width() const;
    uint16_t             height() const;

    int16_t y() const;
    int16_t x() const;

protected:
    SnapshotBitmapFormat m_format;

    int16_t m_x{0},
        m_y{0};
    uint16_t m_width{0},
        m_height{0};
};

class FT8xx;

class RamG
{
public:
    RamG(FT8xx * parent, uint32_t size);

    /*!
     * \brief Saved curent Ram_DL data to Ram_G for next using with append(...) function for reduce SPI overhead
     * \param name Display list name
     * \return pointer to display list memory object
     */
    DisplayList * saveDisplayList(string name) const;
    inline void   removeDisplayList(DisplayList * list) const
    {
        removeStoredObject(list);
    }
    inline void removeDisplayList(std::string name) const
    {
        removeStoredObject(name);
    }
    DisplayList * updateDisplayList(DisplayList * list) const;

    Snapshot * saveSnapshot(string               name,
                            SnapshotBitmapFormat fmt    = ARGB4_s,
                            int16_t              x      = 0,
                            int16_t              y      = 0,
                            uint16_t             width  = EVE_HSIZE,
                            uint16_t             height = EVE_VSIZE) const;

    inline void removeSnapshot(Snapshot * sn) const
    {
        removeStoredObject(sn);
    }
    void removeSnapshot(std::string name) const
    {
        removeStoredObject(name);
    }
    Snapshot * updateSnapshot(Snapshot * s) const;

    const std::vector<StoredObject *> & pool() const;

private:
    /* Raw memory commands. Users actually does'n use in directly.
     * To opperate with Ram_G call FT8XX::RamGInit() and work with public
     * memory commands */
    void    memCopy(uint32_t dest, uint32_t src, uint32_t num) const;
    void    memZero(uint32_t ptr, uint32_t num) const;
    void    memSet(uint32_t ptr, uint8_t value, uint32_t num) const;
    void    removeStoredObject(StoredObject * o) const;
    void    removeStoredObject(std::string name) const;
    void    alignMemory() const;
    void    findMemGap();
    FT8xx * m_parent;

    uint32_t m_start{0x0},
        m_size{0x0};
    mutable uint32_t                    m_currentPosition{0x0};
    mutable std::vector<StoredObject *> m_pool;
};

#if defined(BT81X_ENABLE)
class Flash
{
public:
    Flash(FT8xx * parent, uint32_t size);

    FlashStatus flashStatus() const;

private:
    FT8xx *     m_parent;
    uint32_t    m_size{0};
    FlashStatus m_flashStatus{FLASH_STATUS_DETACHED};
};
#endif
}    // namespace EVE

#endif    // FT8XXMEMORY_H

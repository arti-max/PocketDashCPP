local M = {}

-- Определение размеров спрайтов в спрайтшите
local SPRITE_SIZE = 16
local PORTAL_SIZE_Y = 32
local PORTAL_SIZE_x = 16

local options ={
textureMagFilter = "nearest",
textureMinFilter = "nearest",
pixelScale = "nearest"
}

-- Определяем все спрайты из PD-gameSheet.png
M.spriteOptions = {
    frames = {
        -- Player_01 [DEF] [1]
        {
            x = 464,
            y = 16,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- Player_02 [DEF] [2]
        {
            x = 480,
            y = 16,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- Block_01 [3]
        {
            x = 0,
            y = SPRITE_SIZE,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },
        
        -- Spikes_01 [4]
        {
            x = SPRITE_SIZE,
            y = SPRITE_SIZE,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- Orb_01 [5]
        {
            x = SPRITE_SIZE * 2,
            y = 0,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- Pad_01 [6]
        {
            x = SPRITE_SIZE * 4,
            y = 13,
            width = SPRITE_SIZE,
            height = 3

        },

        --Portal_B_01 [7]
        {
            x = 0,
            y = SPRITE_SIZE * 2,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE * 3

        },

        -- Portal_Y_02 [8]
        {
            x = SPRITE_SIZE,
            y = SPRITE_SIZE * 2,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE * 3

        },

        -- Trigger_01 [BG] [9]
        {
            x = SPRITE_SIZE * 3,
            y = 0,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE

        },

        -- Block_02 [10]
        {
            x = SPRITE_SIZE * 5,
            y = 0,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE

        },

        -- Spike_02 [11]
        {
            x = SPRITE_SIZE * 7,
            y = 0,
            width = SPRITE_SIZE,
            heigth = SPRITE_SIZE

        },

        -- PauseBtn_01 [12]
        {
            x = 0,
            y = SPRITE_SIZE * 6,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- Editor::BackBlockUI [13]
        {
            x = SPRITE_SIZE,
            y = SPRITE_SIZE*6,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE

        },

        -- Tiny BTN [14]
        {
            x = SPRITE_SIZE,
            y = SPRITE_SIZE*7,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- ShortBTN [15]
        {
            x = SPRITE_SIZE,
            y = SPRITE_SIZE*8,
            width = SPRITE_SIZE*2,
            height = SPRITE_SIZE
        },

        -- MediumBTN [16]
        {
            x = SPRITE_SIZE,
            y = SPRITE_SIZE*9,
            width = SPRITE_SIZE*3,
            height = SPRITE_SIZE
        },

        -- LongBTN [17]
        {
            x = SPRITE_SIZE*4,
            y = SPRITE_SIZE*7,
            width = SPRITE_SIZE*4,
            height = SPRITE_SIZE
        },

        -- Player_10 [18]
        {
            x = SPRITE_SIZE*29,
            y = SPRITE_SIZE*4,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE

        },

        -- Player_11 [19]
        {
            x = SPRITE_SIZE*30,
            y = SPRITE_SIZE*4,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
            
        },

        -- Player_20 [20]
        {
            x = SPRITE_SIZE*29,
            y = SPRITE_SIZE*3,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE

        },

        -- Player_21 [21]
        {
            x = SPRITE_SIZE*30,
            y = SPRITE_SIZE*3,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE

        },

        -- Player_30 [22]
        {
            x = SPRITE_SIZE*29,
            y = SPRITE_SIZE*5,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE

        },

        --Player_31 [23]
        {
            x = SPRITE_SIZE*30,
            y = SPRITE_SIZE*5,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE

        },

        -- Player_40 [24]
        {
            x = SPRITE_SIZE*29,
            y = SPRITE_SIZE*6,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE

        },

        -- Player_41 [25]
        {
            x = SPRITE_SIZE*30,
            y = SPRITE_SIZE*6,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE

        },

        -- OrbCircle [26]
        {
            x = SPRITE_SIZE*2,
            y = SPRITE_SIZE,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE

        },

        -- BigPanel [27]
        {
            x = SPRITE_SIZE,
            y = SPRITE_SIZE*20,
            width = SPRITE_SIZE*9,
            height = SPRITE_SIZE*10
        },

        -- CREATEBTN [28]
        {
            x = SPRITE_SIZE*3,
            y = SPRITE_SIZE*5,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE

        },

        -- SLIDERBTN [29]
        {
            x = SPRITE_SIZE*2,
            y = SPRITE_SIZE*6,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- SLIDERLINE [30]
        {
            x = SPRITE_SIZE*4,
            y = SPRITE_SIZE*6,
            width = SPRITE_SIZE*5,
            height = SPRITE_SIZE
        },

        -- DECO_01 [31]
        {
            x = SPRITE_SIZE*9,
            y = SPRITE_SIZE,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- DECO_02 [32]
        {
            x = SPRITE_SIZE*9,
            y = 0,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- CANCELBTN [33]
        {
            x = SPRITE_SIZE*3,
            y = SPRITE_SIZE*6,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- PLAYBTN [34]
        {
            x = SPRITE_SIZE*4,
            y = SPRITE_SIZE*5,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE

        },

        -- Player_50 [35] {intel}
        {
            x = SPRITE_SIZE*29,
            y = SPRITE_SIZE*7,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- Player_51 [36] {intel}
        {
            x = SPRITE_SIZE*30,
            y = SPRITE_SIZE*7,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- PORTAL1_1 [37]
        {
            x = SPRITE_SIZE*2,
            y = SPRITE_SIZE*2,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE*3
        },

        -- PORTAL2_1 [38]
        {
            x = SPRITE_SIZE*3,
            y = SPRITE_SIZE*2,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE*3
        },

        -- SettingsBtn [39]
        {
            x = SPRITE_SIZE*2,
            y = SPRITE_SIZE*5,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- ArrowLeft [40]
        {
            x = SPRITE_SIZE*5,
            y = SPRITE_SIZE*5,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- ArrowRight [41]
        {
            x = SPRITE_SIZE*6,
            y = SPRITE_SIZE*5,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- CheckBox BG [42]
        {
            x = 0,
            y = SPRITE_SIZE*8,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- checkmark [43]
        {
            x = 0,
            y = SPRITE_SIZE*5+1,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE-1
        },

        -- cross [44]
        {
            x = SPRITE_SIZE,
            y = SPRITE_SIZE*5,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- PKDLOGO [45]
        {
            x = 251,
            y = 454,
            width = 244,
            height = 27
        },

        -- MediumBTN2 [46]
        {
            x = SPRITE_SIZE,
            y = SPRITE_SIZE*10,
            width = SPRITE_SIZE*3,
            height = SPRITE_SIZE

        },

        -- DeleteLevel [47]
        {
            x = SPRITE_SIZE*4,
            y = SPRITE_SIZE*8,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- EditLevel [48]
        {
            x = SPRITE_SIZE*4,
            y = SPRITE_SIZE*9,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- UploadLevel [49]
        {
            x = SPRITE_SIZE*3,
            y = SPRITE_SIZE*8,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- ARROW3 [50]
        {
            x = SPRITE_SIZE*7,
            y = SPRITE_SIZE*5,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- ARROW4 [51]
        {
            x = SPRITE_SIZE*8,
            y = SPRITE_SIZE*5,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- BLOCK3 [52]
        {
            x = SPRITE_SIZE*4,
            y = SPRITE_SIZE, 
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- DECO3 [53]
        {
            x = SPRITE_SIZE*10,
            y = SPRITE_SIZE,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- ROUNDED BTN [54]
        {
            x = SPRITE_SIZE*6,
            y = SPRITE_SIZE*8,
            width = SPRITE_SIZE*2,
            height = SPRITE_SIZE*2

        },

        -- Play Logo [55]
        {
            x = SPRITE_SIZE*24,
            y = SPRITE_SIZE*2,
            width = SPRITE_SIZE*2,
            height = SPRITE_SIZE*2
        },

        -- Online Logo [56]
        {
            x = SPRITE_SIZE*22,
            y = 0,
            height = SPRITE_SIZE*2,
            width = SPRITE_SIZE*2
        },

        -- Editor Logo [57]
        {
            x = SPRITE_SIZE*22,
            y = SPRITE_SIZE*2,
            width = SPRITE_SIZE*2,
            height = SPRITE_SIZE*2
        },

        -- Skins Logo [58]
        {
            x = SPRITE_SIZE*24,
            y = 0,
            width = SPRITE_SIZE*2,
            height = SPRITE_SIZE*2
        },

        -- Fill Skin [59]
        {
            x = SPRITE_SIZE*29,
            y = 0,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- SettingsLogo [60]
        {
            x = SPRITE_SIZE*20,
            y = 0,
            width = SPRITE_SIZE*2,
            height = SPRITE_SIZE*2
        },

        -- StatsLogo [61]
        {
            x = SPRITE_SIZE*20,
            y = SPRITE_SIZE*2,
            width = SPRITE_SIZE*2,
            height = SPRITE_SIZE*2
        },

        -- LikeHeart [62]
        {
            x = SPRITE_SIZE*13,
            y = SPRITE_SIZE*4,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- DiffNA [63]
        {
            x = SPRITE_SIZE*9,
            y = SPRITE_SIZE*5,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- DiffEASY [64]
        {
            x = SPRITE_SIZE*10,
            y = SPRITE_SIZE*5,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- DiffNORMAL [65]
        {
            x = SPRITE_SIZE*11,
            y = SPRITE_SIZE*5,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- DiffHARD [66]
        {
            x = SPRITE_SIZE*12,
            y = SPRITE_SIZE*5,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- Lock [67]
        {
            x = SPRITE_SIZE*28,
            y = 0,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- SearchIco [68]
        {
            x = SPRITE_SIZE*9,
            y = SPRITE_SIZE*4,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        },

        -- SkinSelection [69]
        {
            x = SPRITE_SIZE*9,
            y = SPRITE_SIZE*6,
            width = SPRITE_SIZE,
            height = SPRITE_SIZE
        }

    }
}

-- Индексы спрайтов для удобного доступа
M.sprites = {
    -- skins :
    FILLSKIN = 59,
    PLAYER_01 = 1,
    PLAYER_02 = 2,
    PLAYER_10 = 18,
    PLAYER_11 = 19,
    PLAYER_20 = 20,
    PLAYER_21 = 21,
    PLAYER_30 = 22,
    PLAYER_31 = 23,
    PLAYER_40 = 24,
    PLAYER_41 = 25,
    PLAYER_50 = 35,
    PLAYER_51 = 36,

    -- UI :
    PAUSE1 = 12,
    BLOCKUI1 = 13,
    TINYBTN = 14,
    SHORTBTN = 15,
    MEDIUMBTN = 16,
    LONGBTN = 17,
    BIGPANEL = 27,
    CREATEBTN = 28,
    SLIDERBTN = 29,
    SLIDERLINE = 30,
    CANCELBTN = 33,
    PLAYBTN = 34,
    SETTINGSBTN = 39,
    ARROW1 = 40,
    ARROW2 = 41,
    CHECKBOX = 42,
    CHECKMARK = 43,
    CROSS = 44,
    POCKETDASHLOGO = 45,
    MEDIUMBTN2 = 46,
    DELETELEVEL = 47,
    EDITLEVEL = 48,
    UPLOADLEVEL = 49,
    ARROW3 = 50,
    ARROW4 = 51,
    ROUNDEDBTN = 54,
    PLAYLOGO = 55,
    ONLINELOGO = 56,
    EDITORLOGO = 57,
    SKINSLOGO = 58,
    SETTINGSLOGO = 60,
    STATSLOGO = 61,
    LIKE = 62,
    LOCK = 67,
    SEARCHICO = 68,
    SKINSELECTION = 69,

    -- other :
    ORBCIRCLE = 26,

    -- blocks :

    BLOCK1 = 3,
    SPIKE1 = 4,
    ORB1 = 5,
    PAD1 = 6,
    PORTAL1 = 7,
    PORTAL2 = 8,
    TRIGGER1 = 9,
    BLOCK2 = 10,
    SPIKE2 = 11,
    DECO1 = 31,
    DECO2 = 32,
    PORTAL1_1 = 37,
    PORTAL2_1 = 38,
    BLOCK3 = 52,
    DECO3 = 53,

    -- online :
    DIFF_NA = 63,
    DIFF_EASY = 64,
    DIFF_NORMAL = 65,
    DIFF_HARD = 66,
}

M.skins = {
    ['color'] = M.sprites.FILLSKIN,
    ['lock'] = M.sprites.LOCK,
    ['1'] = M.sprites.PLAYER_01,
    ['11'] = M.sprites.PLAYER_02,
    ['2'] = M.sprites.PLAYER_10,
    ['21'] = M.sprites.PLAYER_11,
    ['3'] = M.sprites.PLAYER_20,
    ['31'] = M.sprites.PLAYER_21,
    ['4'] = M.sprites.PLAYER_30,
    ['41'] = M.sprites.PLAYER_31,
    ['5'] = M.sprites.PLAYER_40,
    ['51'] = M.sprites.PLAYER_41,
    ['6'] = M.sprites.PLAYER_50,
    ['61'] = M.sprites.PLAYER_51

}

M.blocks = {
    ['1'] = M.sprites.BLOCK1,
    ['2'] = M.sprites.SPIKE1,
    ['3'] = M.sprites.ORB1,
    ['4'] = M.sprites.PAD1,
    ['5'] = M.sprites.PORTAL1,
    ['6'] = M.sprites.PORTAL2,
    ['7'] = M.sprites.TRIGGER1,
    ['8'] = M.sprites.BLOCK2,
    ['9'] = M.sprites.SPIKE2,
    ['10'] = M.sprites.DECO1,
    ['11'] = M.sprites.DECO2,
    ['12'] = M.sprites.BLOCK3,
    ['13'] = M.sprites.DECO3

}

-- Функция для создания imageSheet
function M.init()
    M.imageSheet = graphics.newImageSheet("PD-gameSheet.png", M.spriteOptions, options)
    return M.imageSheet
end

-- Функция для создания спрайта
function M.newSprite(spriteType, width, height)
    if not M.imageSheet then
        M.init()
    end

    local sprite = display.newImageRect(M.imageSheet, spriteType, width or 30, height or 30)
    sprite._lastEqn = "nearest" -- Отключаем сглаживание
    return sprite
end

return M
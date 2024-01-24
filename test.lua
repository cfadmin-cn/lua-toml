require "utils"
local toml = require "toml"

local tab = toml.load ([[
# This is a TOML document

title = "TOML Example"

[owner]
vname = ""
name = "Tom Preston-Werner"
dob = 1979-05-27T07:32:00-08:00

[database]
enabled = true
ports = [ 8000, 800_1, 80_02, 8_003, 8_00_4, 8_0_0_5 ]
data = [ ["delta", "phi"], [3.14] ]
temp_targets = { cpu = 79.5, case = 72.0, disk = 64.3 }

[servers]

[servers.alpha]
ip = "10.0.0.1"
role = "frontend"

[servers.beta]
ip = "10.0.0.2"
role = "backend"
]])

var_dump(tab)

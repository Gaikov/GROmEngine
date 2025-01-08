const color = process.argv[2]
if (!color) {
    console.warn("Color is not specified!");
    process.exit(-1);
}

console.log("...converting:", color);

const value = parseInt(color, 16);
console.log(value);

function comp(value, precision) {
    return Math.round((value & 0xff) * precision / 0xff) / precision;
}

const a = comp(value, 1000);
const b = comp(value >> 8, 1000);
const g = comp(value >> 16, 1000);
const r = comp(value >> 24, 1000);

console.log("rgba: ", r, g, b, a);
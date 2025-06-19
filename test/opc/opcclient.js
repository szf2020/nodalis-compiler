const readline = require("readline");
const {
    OPCUAClient,
    AttributeIds,
    DataType,
    StatusCodes
} = require("node-opcua");

(async () => {
    const rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout
    });

    function askQuestion(query) {
        return new Promise(resolve => rl.question(query, resolve));
    }

    const endpointUrl = await askQuestion("Enter OPC UA server URL (e.g. opc.tcp://localhost:4334): ");
    const client = OPCUAClient.create({ endpoint_must_exist: false });

    try {
        await client.connect(endpointUrl.trim());
        const session = await client.createSession();
        console.log("Connected to", endpointUrl);

        async function handleCommand(commandLine) {
            const [command, variable, value] = commandLine.trim().split(/\s+/);
            const nodeId = `ns=1;s=${variable}`;

            if (command === "read") {
                const dataValue = await session.read({
                    nodeId,
                    attributeId: AttributeIds.Value
                });
                console.log(`${variable} = ${dataValue.value.value}`);
            } else if (command === "write") {
                const boolValue = value === "true";
                const statusCode = await session.write({
                    nodeId,
                    attributeId: AttributeIds.Value,
                    value: {
                        value: { dataType: DataType.Boolean, value: boolValue }
                    }
                });
                if (statusCode === StatusCodes.Good) {
                    console.log(`Wrote ${boolValue} to ${variable}`);
                } else {
                    console.log("Write failed:", statusCode.toString());
                }
            } else {
                console.log("Unknown command. Use 'read VARIABLE' or 'write VARIABLE true/false'");
            }
        }

        function promptLoop() {
            rl.question("> ", async (line) => {
                if (line.trim().toLowerCase() === "exit") {
                    rl.close();
                    await session.close();
                    await client.disconnect();
                    console.log("Disconnected.");
                    process.exit(0);
                } else {
                    await handleCommand(line);
                    promptLoop();
                }
            });
        }

        promptLoop();
    } catch (err) {
        console.error("Error:", err);
        await client.disconnect();
        rl.close();
    }
})();

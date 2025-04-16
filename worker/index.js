addEventListener('fetch', event => {
    event.respondWith(handleRequest(event.request));
});

async function handleRequest(request) {
    try {
        const contentType = request.headers.get('content-type');
        let data;

        if (contentType && contentType.includes('application/json')) {
            data = await request.json();
        } else {
            data = await request.text(); //get the body as text
        }

        // Log the data (for debugging and monitoring)
        console.log('Received data:', data);

        // Process the data (example: store in KV)
        if (data) {
            // Example: Store in Cloudflare KV (replace with your KV namespace)
            await MY_KV_NAMESPACE.put(Date.now().toString(), JSON.stringify(data));
        }

        // Construct a response
        const responseBody = {
            message: 'Data received successfully!',
            receivedData: data,
            // You can add other information here, like a timestamp
            timestamp: new Date().toISOString()
        };

        // Return the response
        return new Response(JSON.stringify(responseBody), {
            headers: { 'Content-Type': 'application/json' },
            status: 200 // OK
        });
    } catch (error) {
        console.error('Error processing request:', error);
        return new Response(JSON.stringify({ error: 'Internal server error', message: error.message }), {
            headers: { 'Content-Type': 'application/json' },
            status: 500 // Internal Server Error
        });
    }
}

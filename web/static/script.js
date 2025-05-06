document.addEventListener('DOMContentLoaded', function () {
  const joystickContainer = document.getElementById('joystickContainer');
  const joystick = document.getElementById('joystick');
  const pointer = document.getElementById('pointer');
  const angleDisplay = document.getElementById('angleDisplay');

  let isDragging = false;
  let containerRect = joystickContainer.getBoundingClientRect();
  let containerCenterX = containerRect.width / 2;
  let containerCenterY = containerRect.height / 2;
  let maxDistance = containerRect.width / 2 - 30; // Raio máximo de movimento

  // Atualiza o retângulo do contêiner quando a janela é redimensionada
  window.addEventListener('resize', function () {
      containerRect = joystickContainer.getBoundingClientRect();
      containerCenterX = containerRect.width / 2;
      containerCenterY = containerRect.height / 2;
      maxDistance = containerRect.width / 2 - 30;
  });

  function handleStart(e) {
      isDragging = true;
      handleMove(e);
  }

  function handleMove(e) {
      if (!isDragging) return;

      e.preventDefault();

      // Obter coordenadas do toque ou mouse
      const clientX = e.clientX || (e.touches && e.touches[0].clientX);
      const clientY = e.clientY || (e.touches && e.touches[0].clientY);

      if (clientX === undefined || clientY === undefined) return;

      const containerRect = joystickContainer.getBoundingClientRect();
      const x = clientX - containerRect.left;
      const y = clientY - containerRect.top;

      // Calcular distância do centro
      const deltaX = x - containerCenterX;
      const deltaY = y - containerCenterY;
      const distance = Math.sqrt(deltaX * deltaX + deltaY * deltaY);

      // Limitar movimento dentro do contêiner
      let limitedX, limitedY;
      if (distance > maxDistance) {
          const angle = Math.atan2(deltaY, deltaX);
          limitedX = containerCenterX + maxDistance * Math.cos(angle);
          limitedY = containerCenterY + maxDistance * Math.sin(angle);
      } else {
          limitedX = x;
          limitedY = y;
      }

      // Mover o joystick
      joystick.style.left = `${limitedX}px`;
      joystick.style.top = `${limitedY}px`;
      joystick.style.transform = 'translate(-50%, -50%)';

      // Calcular ângulo para o ponteiro
      const angle = Math.atan2(limitedY - containerCenterY, limitedX - containerCenterX);
      const degrees = (angle * 180 / Math.PI) + 90; // +90 para alinhar com o norte

      // Atualizar o ponteiro
      pointer.style.transform = `translate(-50%, -100%) rotate(${degrees}deg)`;

      // Atualizar o display de ângulo
      const displayDegrees = Math.round((degrees + 360) % 360);
      angleDisplay.textContent = `Ângulo: ${displayDegrees}°`;
  }

  function handleEnd() {
      if (!isDragging) return;
      isDragging = false;

      // Retornar o joystick ao centro
      joystick.style.left = '50%';
      joystick.style.top = '50%';
      joystick.style.transform = 'translate(-50%, -50%)';

      // Resetar o ponteiro para o norte (0 graus)
      pointer.style.transform = 'translate(-50%, -100%) rotate(0deg)';
      angleDisplay.textContent = 'Ângulo: 0°';
  }

  // Eventos de mouse
  joystick.addEventListener('mousedown', handleStart);
  document.addEventListener('mousemove', handleMove);
  document.addEventListener('mouseup', handleEnd);

  // Eventos de toque
  joystick.addEventListener('touchstart', handleStart);
  document.addEventListener('touchmove', handleMove, { passive: false });
  document.addEventListener('touchend', handleEnd);

const socket = io();

// Recebe o comando do servidor com o ângulo
socket.on('command', (data) => {
  console.log("Recebido via Socket.IO:", data);

  if (data != null) {
    const angle = data.angulo;

    // Atualiza visualmente o ponteiro
    pointer.style.transform = `translate(-50%, -100%) rotate(${angle}deg)`;

    // Atualiza o texto com o ângulo
    angleDisplay.textContent = `Ângulo: ${Math.round(angle)}°`;
  } else {
    console.warn("Dado de ângulo inválido recebido:", data);
  }
});
});